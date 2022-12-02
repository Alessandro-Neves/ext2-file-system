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
  this->blocks_group_descriptor = read_ext2_blocks_group_descriptor(this->ext2_image, block_group_descriptor_address(0));
  Ext2_Inode* first_inode = read_ext2_inode(this->ext2_image, this->blocks_group_descriptor, 2);
  vector<Ext2_Directory> directories = read_ext2_directories(this->ext2_image, first_inode);
  this->history_navigation.push_back(directories.at(0));
}

bool Ext2FileManager::cd(const char* directory_name) {

  if(!std::strcmp(directory_name, ".")) {
    print_directory(this->history_navigation.back());
    return true;
  }
  if(!std::strcmp(directory_name, "..") && this->history_navigation.size() == 1)  {
    std::cout << std::string(BOLD) << "no directories to go back" << std::string(DEFAULT) << std::endl;
    return true;
  }
  if(!std::strcmp(directory_name, "..")) {
    this->history_navigation.pop_back();
    Ext2_Directory actual_directory = this->history_navigation.back();
    unsigned int directory_inode_block_group = block_group_from_inode(this->superblock, actual_directory.inode);
    this->blocks_group_descriptor = read_ext2_blocks_group_descriptor(this->ext2_image, block_group_descriptor_address(directory_inode_block_group));
    print_directory(this->history_navigation.back());
    return true;
  }

  Ext2_Directory actual_directory = this->history_navigation.at(this->history_navigation.size() - 1);
  Ext2_Inode* actual_inode = read_ext2_inode(this->ext2_image, this->blocks_group_descriptor, inode_order_on_block_group(this->superblock, actual_directory.inode));
  Ext2_Directory* directory = search_directory(this->ext2_image, actual_inode, directory_name);

  if(!directory) return false;

  unsigned int directory_inode_block_group = block_group_from_inode(this->superblock, directory->inode);
  this->blocks_group_descriptor = read_ext2_blocks_group_descriptor(this->ext2_image, block_group_descriptor_address(directory_inode_block_group));

  this->history_navigation.push_back(*directory);
  print_directory(this->history_navigation.back());

  return true;
}

void Ext2FileManager::ls() {
  Ext2_Directory actual_directory = this->history_navigation.at(this->history_navigation.size() - 1);
  Ext2_Inode* actual_inode = read_ext2_inode(this->ext2_image, this->blocks_group_descriptor, inode_order_on_block_group(this->superblock, actual_directory.inode));
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
}

void Ext2FileManager::info_inode(unsigned int inode) {
  unsigned int inode_block_group = block_group_from_inode(this->superblock, inode);
  Ext2_Blocks_Group_Descriptor *blocks_group_descriptor_of_inode = read_ext2_blocks_group_descriptor(this->ext2_image, block_group_descriptor_address(inode_block_group));
  Ext2_Inode* found_inode = read_ext2_inode(this->ext2_image, blocks_group_descriptor_of_inode, inode);
  print_ext2_inode(found_inode);
}

std::string Ext2FileManager::pwd() {
  std::string str;
  for(vector<Ext2_Directory>::iterator it = this->history_navigation.begin(); it != this->history_navigation.end(); it++){
    if(std::strcmp((*it).name, ".")) str = str.append((*it).name);
    str = str.append("/");
  }

  return str;
}