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

  public:
  void info_superblock();
  void info_blocks_group_descriptor();
  void info_inode(unsigned int inode);
  void info();

  public:
  bool cd(const char *directory_name);
  bool cat(const char *directory_name);
  bool attr(const char *directory_name);
  void touch(const char *directory_name, unsigned int directory_name_length);
  bool rename(const char *directory_name, const char *new_directory_name, unsigned int new_directory_name_length);
  void ls();
  std::string pwd();
  Ext2FileManager(FILE *ext2_image);
};

#endif