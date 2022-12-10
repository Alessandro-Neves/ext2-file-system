#include <sys/stat.h> /*S_ISDIR  */

#include "../headers/directory-operations.hpp"
#include "../headers/colors.hpp"

#include "../headers/general-constants.hpp"

vector<Ext2_Directory> read_ext2_directories(FILE* ext2_image, Ext2_Inode* inode) {

  vector<Ext2_Directory> directories;

  if(S_ISDIR(inode->i_mode)) {

    for(int i = 0; i < 12; i++){
      Block* block = (Block*)malloc(sizeof(Block));

      if(!inode->i_block[i]) break;

      fseek(ext2_image, BLOCK_OFFSET(inode->i_block[i]), SEEK_SET);
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
  }

  return directories;
}

Ext2_Directory* search_directory(FILE* ext2_image, Ext2_Inode* inode, const char* searched_name){
  Ext2_Directory* directory = NULL;

  vector<Ext2_Directory> directories = read_ext2_directories(ext2_image, inode);
  
  for(vector<Ext2_Directory>::iterator it = directories.begin(); it != directories.end(); it++){
    const char* iterator_directory_name = (const char*) (*it).name;
    if(!strcmp(searched_name, iterator_directory_name)){
      directory = &(*it);
      break;
    }
  }

  return directory;
}

Ext2_Directory* search_directory_and_position(FILE* ext2_image, Ext2_Inode* inode, const char* searched_name, unsigned int* store_position_of_directory_on_block){
  Ext2_Directory* directory = NULL;

  vector<Ext2_Directory> directories = read_ext2_directories(ext2_image, inode);

  unsigned int position = 0;
  
  for(vector<Ext2_Directory>::iterator it = directories.begin(); it != directories.end(); it++){
    const char* iterator_directory_name = (const char*) (*it).name;
    if(!strcmp(searched_name, iterator_directory_name)){
      directory = (Ext2_Directory*) malloc(sizeof(Ext2_Directory));
      memcpy(directory, &(*it), sizeof(Ext2_Directory));
      break;
    }

    position += (*it).rec_len;
  }

  if(directory) (*store_position_of_directory_on_block) = position;
    
  return directory;
}

void print_directory(Ext2_Directory directory){
  cout << string(BOLD) <<  directory.name << string(DEFAULT) << endl;
  cout << "inode:\t\t\t" << (unsigned) directory.inode << endl;
  cout << "record lenght:\t\t" << (unsigned) directory.rec_len << endl;
  cout << "name lenght:\t\t" << (unsigned) directory.name_len << endl;
  cout << "file type:\t\t" << (unsigned) directory.file_type << endl;
}

void print_directories(vector<Ext2_Directory> directories) {
  for(vector<Ext2_Directory>::iterator it = directories.begin(); it != directories.end(); it++){
    print_directory(*it);
    cout << endl;
  }
}