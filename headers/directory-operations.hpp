#include "./ext2.hpp"

#ifndef _DIRECTORY_OPERATIONS_H_
#define _DIRECTORY_OPERATIONS_H_

using namespace std;

vector<Ext2_Directory> read_ext2_directories(FILE* ext2_image, Ext2_Inode* inode);

Ext2_Directory* search_directory(FILE* ext2_image, Ext2_Inode* inode, const char* searched_name);

void print_directory(Ext2_Directory directory);

void print_directories(vector<Ext2_Directory> directories);

#endif