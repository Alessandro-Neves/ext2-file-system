#include <cstdbool>
#include "./ext2.hpp"
#include "./general-constants.hpp"

using namespace std;

#ifndef _EXT2_FILE_MANAGER_H_
#define _EXT2_FILE_MANAGER_H_

class Ext2FileManager {
  private:
  FILE *ext2_image = NULL;
  Ext2_Superblock *superblock = NULL;
  Ext2_Blocks_Group_Descriptor *blocks_group_descriptor = NULL;
  vector<Ext2_Directory> history_navigation;

  Ext2_Inode* _read_ext2_inode(uint32_t inode);
  void release_blocks(uint32_t *blocks, int blocks_size, uint32_t *bytes_to_remove);
  void release_blocks_of_inode(Ext2_Inode* inode);


  public:
  void info_superblock();
  void info_blocks_group_descriptor(int bgd_index);
  void info_inode(unsigned int inode);

  

  public:
  void cd(const char *directory_name);
  void cat(const char *directory_name);
  void copy(const char *origin_name, const char *destiny_name, bool info);
  void move(const char *origin_name, const char* destiny_name);
  void touch(const char *directory_name, unsigned int directory_name_length);
  bool rename(const char *directory_name, const char *new_directory_name, unsigned int new_directory_name_length);

  bool rm(const char *directory_name, unsigned int directory_name_length, bool info);
  void ls();

  public:
  void set_bit_of_inode_bitmap(unsigned int inode, bool value);
  void set_bit_of_block_bitmap(unsigned int block, bool value);
  void print_block_bitmap(unsigned int inode);
  void print_byte_on_bitmap_of_inode(unsigned int inode);
  void print_byte_on_bitmap_of_block(unsigned int block);
  void test();

  std::string pwd();
  Ext2FileManager(FILE *ext2_image);
};

#endif