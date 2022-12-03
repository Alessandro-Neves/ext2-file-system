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

Ext2_Inode *read_ext2_inode(FILE *ext2_image, Ext2_Blocks_Group_Descriptor *block_group_descriptor, unsigned int inode_order)
{

  Ext2_Inode *inode = (Ext2_Inode*) malloc(sizeof(Ext2_Inode));

  int inode_position = BLOCK_OFFSET(block_group_descriptor->bg_inode_table) + ( (inode_order - 1) * sizeof(struct ext2_inode) );

  fseek(ext2_image, inode_position, SEEK_SET);

  fread(inode, 1, sizeof(Ext2_Inode), ext2_image);
  return inode;
}

unsigned int inode_order_on_block_group(Ext2_Superblock* superblock, uint32_t inode) {
  return (unsigned int) inode % superblock->s_inodes_per_group;
}

void print_ext2_inode(Ext2_Inode* inode){
  cout << "i_mode:\t\t\t"             << inode->i_mode              << endl;
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

static unsigned int last_position_of_content_on_block(unsigned int bytes_to_read) {
  return bytes_to_read >= BLOCK_SIZE ? BLOCK_SIZE : bytes_to_read;
}

void print_inode_blocks_content(FILE* ext2_image, Ext2_Inode* inode) {

  char* content = (char*) malloc (sizeof(char) * (BLOCK_SIZE + 1));
  int position;

  unsigned int bytes_to_read = inode->i_size;

  unsigned int blocks_read = 0;

  bool exit = false;

  system("clear");
  cout << string(BLUE) << "[ " << (blocks_read / 10) + 1 << " of " << bytes_to_read / ((BLOCK_SIZE * 10)) + 1 << " ]" << string(DEFAULT) << endl << endl;
  
  for(int i = 0; i < 12; i++){
    if(bytes_to_read <= BLOCK_SIZE) exit = true;

    if(blocks_read % 10 == 0 && blocks_read > 0){
      cout << string(GREEN) << endl << "[ press ANY key to read more, ESQ to exit ]\t" << string(DEFAULT);
      system("stty raw"); 
      if(getchar() == 27) {
        system("stty cooked");
        system("clear");
        return;
      } 
      system("stty cooked");
      system("clear");
      cout << string(BLUE) << "[ " << (blocks_read / 10) + 1 << " of " << bytes_to_read / ((BLOCK_SIZE * 10)) + 1 << " ]" << string(DEFAULT) << endl << endl;
    }

    position = BLOCK_OFFSET((inode->i_block)[i]);
    fseek(ext2_image, position, SEEK_SET);
    fread(content, 1, BLOCK_SIZE, ext2_image);

    content[last_position_of_content_on_block(bytes_to_read)] = '\0';

    cout << content << endl;

    if(exit) {
      cout << string(YELLOW) << "[ end of file, press any key to exit ]\t" << string(DEFAULT);
      cin.get();
      system("clear");
      return;
    };
    bytes_to_read -= 1024;
    blocks_read ++;
  }

  uint32_t *indexes_level_1 = (uint32_t*) malloc(sizeof(uint32_t) * 256);

  position = BLOCK_OFFSET((inode->i_block)[12]);

  fseek(ext2_image, position, SEEK_SET);
  fread(indexes_level_1, 1, BLOCK_SIZE, ext2_image);

  for(int i = 0; i < 256; i++){
    if(bytes_to_read <= BLOCK_SIZE) exit = true;

    if(blocks_read % 10 == 0 && blocks_read > 0){
      cout << string(GREEN) << endl << "[ press ANY key to read more, ESQ to exit ]\t" << string(DEFAULT);
      system("stty raw"); 
      if(getchar() == 27) {
        system("stty cooked");
        system("clear");
        return;
      }
      system("stty cooked");
      system("clear");
      cout << string(BLUE) << "[ " << (blocks_read / 10) + 1 << " of " << bytes_to_read / ((BLOCK_SIZE * 10)) + 1 << " ]"<< string(DEFAULT) << endl << endl;
    }

    position = BLOCK_OFFSET(indexes_level_1[i]);
    fseek(ext2_image, position, SEEK_SET);
    fread(content, 1, BLOCK_SIZE, ext2_image);

    content[last_position_of_content_on_block(bytes_to_read)] = '\0';

    cout << content << endl;

    if(exit) {
      cout << string(YELLOW) << "[ end of file, press any key to exit ]" << string(DEFAULT);
      cin.get();
      system("clear");
      return;
    };

    bytes_to_read -= 1024;

    blocks_read ++;
  }


  /**
   * @todo leitura blocos inodes indiretos (13+)
   * 
   */
}