#include <sys/stat.h> /*S_ISDIR  */

#include "../headers/directory-operations.hpp"
#include "../headers/colors.hpp"

#include "../headers/general-constants.hpp"

vector<Ext2_Directory> read_ext2_directories(FILE* ext2_image, Ext2_Inode* inode, Ext2_Blocks_Group_Descriptor* block_group_descriptor) {

  vector<Ext2_Directory> directories;

  if(S_ISDIR(inode->i_mode)) {

    Block* block = (Block*)malloc(sizeof(Block));

    fseek(ext2_image, BLOCK_OFFSET(inode->i_block[0]), SEEK_SET);
    fread(block, 1, sizeof(Block), ext2_image);

    
    Ext2_Directory* worked_directory = (Ext2_Directory *) block;
    int block_position = 0;
    do{
      Ext2_Directory* directory = (Ext2_Directory*) malloc(sizeof(Ext2_Directory));
      memcpy(directory, worked_directory, sizeof(Ext2_Directory));
      (directory->name)[directory->name_len] = '\0';
      directories.push_back(*directory);

			worked_directory = (Ext2_Directory *) ((char*) worked_directory + worked_directory->rec_len);
			block_position += worked_directory->rec_len;
    }while((block_position < inode->i_size) && worked_directory->inode);

    free(block);
  }

  return directories;
}

void print_directories(vector<Ext2_Directory> directories) {
  for(vector<Ext2_Directory>::iterator it = directories.begin(); it != directories.end(); it++){
    cout << setw(6) << (unsigned) (*it).inode  << "\t\t" <<  (*it).name  << endl;
  }
}