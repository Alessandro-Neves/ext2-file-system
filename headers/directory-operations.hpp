#include "./ext2.hpp"

#ifndef _DIRECTORY_OPERATIONS_H_
#define _DIRECTORY_OPERATIONS_H_

using namespace std;

vector<Ext2_Directory> read_ext2_directories(FILE* ext2_image, Ext2_Inode* inode, Ext2_Blocks_Group_Descriptor* block_group_descriptor);

void print_directories(vector<Ext2_Directory> directories);

#endif