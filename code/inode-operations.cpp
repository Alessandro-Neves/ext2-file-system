#include <iostream>
#include "../headers/inode-operations.hpp"
#include "../headers/colors.hpp"

using namespace std;

Ext2_Inode *read_ext2_inode(FILE *ext2_image, Ext2_Blocks_Group_Descriptor *block_group_descriptor)
{
  Ext2_Inode *inode = (Ext2_Inode*) malloc(sizeof(Ext2_Inode));

  fseek(ext2_image, BLOCK_OFFSET(block_group_descriptor->bg_inode_table), SEEK_SET);

  fread(inode, 1, sizeof(Ext2_Inode), ext2_image);
  return inode;
}

void print_ext2_inode(Ext2_Inode* inode){
  cout << string(BLUE) << endl << "[ Inode infos ]" << string(DEFAULT) << endl;
  cout << "i_mode:\t\t\t"             << inode->i_mode              << endl;
  cout << "i_uid:\t\t\t"              << inode->i_uid               << endl;
  cout << "i_size:\t\t\t"             << inode->i_size              << endl;
  cout << "i_atime:\t\t"              << inode->i_atime             << endl;
  cout << "i_ctime:\t\t"              << inode->i_ctime             << endl;
  cout << "i_mtime:\t\t"              << inode->i_mtime             << endl;
  cout << "i_dtime:\t\t"              << inode->i_dtime             << endl;
  cout << "i_gid:\t\t\t"              << inode->i_gid               << endl;
  cout << "i_links_count:\t\t"        << inode->i_links_count       << endl;
  cout << "i_gid:\t\t\t"              << inode->i_gid               << endl;
  cout << "i_flags:\t\t"              << inode->i_flags             << endl;
  cout << "i_blocks:\t\t"             << inode->i_blocks            << endl;
  cout << "osd1:\t\t\t"               << inode->osd1                << endl;
  cout << "i_block:\t\t"              << "[ " << inode->i_block[0] << " ... ][15]"    << endl;
  cout << "i_generation:\t\t"         << inode->i_generation        << endl;
  cout << "i_file_acl:\t\t"           << inode->i_file_acl          << endl;
  cout << "i_dir_acl:\t\t"            << inode->i_dir_acl           << endl;
  cout << "i_faddr:\t\t"              << inode->i_faddr             << endl;
  cout << "i_osd2:\t\t\t"             << "[ " << inode->i_osd2[0] << " ... ][3]"      << endl;
}