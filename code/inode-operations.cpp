#include <iostream>
#include <cstdbool>
#include "../headers/inode-operations.hpp"
#include "../headers/colors.hpp"
#include "../headers/util-operations.hpp"

using namespace std;

void print_array (uint32_t* array, int size) {
  for(int i = 0; i < size; i++)
    cout << " " << array[i];
}

Ext2_Inode *read_ext2_inode(FILE *ext2_image, Ext2_Blocks_Group_Descriptor *block_group_descriptor, int inode_order)
{
  Ext2_Inode *inode = (Ext2_Inode*) malloc(sizeof(Ext2_Inode));

  int inode_position = BLOCK_OFFSET(block_group_descriptor->bg_inode_table) + ( (inode_order - 1) * sizeof(struct ext2_inode) );

  fseek(ext2_image, inode_position, SEEK_SET);

  fread(inode, 1, sizeof(Ext2_Inode), ext2_image);
  return inode;
}

void print_ext2_inode(Ext2_Inode* inode){
  cout << "i_mode:\t\t\t"             << (unsigned) inode->i_mode              << endl;
  cout << "i_uid:\t\t\t"              << (unsigned) inode->i_uid               << endl;
  cout << "i_size:\t\t\t"             << (unsigned) inode->i_size              << endl;
  cout << "i_atime:\t\t"              << (unsigned) inode->i_atime             << endl;
  cout << "i_ctime:\t\t"              << (unsigned) inode->i_ctime             << endl;
  cout << "i_mtime:\t\t"              << (unsigned) inode->i_mtime             << endl;
  cout << "i_dtime:\t\t"              << (unsigned) inode->i_dtime             << endl;
  cout << "i_gid:\t\t\t"              << (unsigned) inode->i_gid               << endl;
  cout << "i_links_count:\t\t"        << (unsigned) inode->i_links_count       << endl;
  cout << "i_gid:\t\t\t"              << (unsigned) inode->i_gid               << endl;
  cout << "i_flags:\t\t"              << (unsigned) inode->i_flags             << endl;
  cout << "i_blocks:\t\t"             << (unsigned) inode->i_blocks            << endl;
  cout << "osd1:\t\t\t"               << (unsigned) inode->osd1                << endl;
  cout << "i_block:\t\t"              << "["; print_array(inode->i_block, 15); cout << " ](15)"    << endl;
  cout << "i_generation:\t\t"         << (unsigned) inode->i_generation        << endl;
  cout << "i_file_acl:\t\t"           << (unsigned) inode->i_file_acl          << endl;
  cout << "i_dir_acl:\t\t"            << (unsigned) inode->i_dir_acl           << endl;
  cout << "i_faddr:\t\t"              << (unsigned) inode->i_faddr             << endl;
  cout << "i_osd2:\t\t\t"             << "["; print_array(inode->i_osd2, 3); cout << " ](3)"      << endl;
}

void print_inode_blocks_content(FILE* ext2_image, Ext2_Inode* inode) {
  char* content = (char*) malloc (sizeof(char) * BLOCK_SIZE);
  int position;

  for(int i = 0; i < 12; i++){
    memset(content, '\0', sizeof(content));
    position = BLOCK_OFFSET((inode->i_block)[0]);

    fseek(ext2_image, position, SEEK_SET);
    fread(content, 1, BLOCK_SIZE, ext2_image);

    cout << content;

    if(has_null(content, BLOCK_SIZE)) break;
  }

  /**
   * @todo leitura blocos inodes indiretos (13+)
   * 
   */
}