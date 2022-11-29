#include "../headers/ext2-file-manager.hpp"
#include "../headers/ext2.hpp"
#include "../headers/file-operations.hpp"
#include "../headers/superblock-operations.hpp"
#include "../headers/blocks-group-operations.hpp"
#include "../headers/inode-operations.hpp"
#include "../headers/general-constants.hpp"
#include "../headers/directory-operations.hpp"

Ext2FileManager::Ext2FileManager(FILE* ext2_image) {
  this->ext2_image = ext2_image;
  this->superblock = read_ext2_superblock(this->ext2_image);
  this->blocks_group_descriptor = read_ext2_blocks_group_descriptor(this->ext2_image, block_group_descriptor_address(1));
  Ext2_Inode* first_inode = read_ext2_inode(this->ext2_image, this->blocks_group_descriptor, 2);
  vector<Ext2_Directory> directories = read_ext2_directories(this->ext2_image, first_inode);
  this->history_navigation.push_back(directories.at(0));
}

bool Ext2FileManager::cd(const char* directory_name) {
  Ext2_Directory actual_directory = this->history_navigation.at(this->history_navigation.size() - 1);
  Ext2_Inode* actual_inode = read_ext2_inode(this->ext2_image, this->blocks_group_descriptor, actual_directory.inode);
  Ext2_Directory* directory = search_directory(this->ext2_image, actual_inode, directory_name);

  if(!directory) return false;

  cout << endl << string(RED) << directory->inode << endl;

  unsigned int directory_inode_block_group = block_group_from_inode(this->superblock, directory->inode);
  this->blocks_group_descriptor = read_ext2_blocks_group_descriptor(this->ext2_image, block_group_descriptor_address(directory_inode_block_group));

  cout << string(RED) << directory_inode_block_group << string(DEFAULT) << endl;

  this->history_navigation.push_back(*directory);

  return true;
}

void Ext2FileManager::ls() {
  Ext2_Directory actual_directory = this->history_navigation.at(this->history_navigation.size() - 1);
  cout << string(RED) << actual_directory.inode << string(DEFAULT) << endl;
  Ext2_Inode* actual_inode = read_ext2_inode(this->ext2_image, this->blocks_group_descriptor, 1);
  vector<Ext2_Directory> directories = read_ext2_directories(this->ext2_image, actual_inode);
  print_directories(directories);
}

bool Ext2FileManager::cat(const char* directory_name) {
  Ext2_Directory actual_directory = this->history_navigation.at(this->history_navigation.size() - 1);
  Ext2_Inode* actual_inode = read_ext2_inode(this->ext2_image, this->blocks_group_descriptor, inode_order_on_block_group(this->superblock, actual_directory.inode));

  Ext2_Directory* directory = search_directory(this->ext2_image, actual_inode, directory_name);

  if(!directory)  return false;

  unsigned int directory_inode_block_group = block_group_from_inode(this->superblock, directory->inode);
  Ext2_Blocks_Group_Descriptor* bgd_of_inode = read_ext2_blocks_group_descriptor(this->ext2_image, block_group_descriptor_address(directory_inode_block_group));

  Ext2_Inode* directory_inode = read_ext2_inode(this->ext2_image, bgd_of_inode, directory->inode);
  print_inode_blocks_content(this->ext2_image, directory_inode);
}

void Ext2FileManager::info_superblock() {
  print_superblock(this->superblock);
}

void Ext2FileManager::info_blocks_group_descriptor() {
  print_ext2_blocks_group_descriptor(this->blocks_group_descriptor);
  cout << endl << string(BOLD) << "blocks_per_group:\t" << blocks_group_count(this->superblock) << string(DEFAULT) << endl;
}

void Ext2FileManager::info_inode() {
  Ext2_Directory actual_directory = this->history_navigation.at(this->history_navigation.size() - 1);
  Ext2_Inode* actual_inode = read_ext2_inode(this->ext2_image, this->blocks_group_descriptor, 1);
  print_ext2_inode(actual_inode);
}