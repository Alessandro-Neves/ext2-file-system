#include "../headers/ext2-file-manager.hpp"
#include "../headers/ext2.hpp"
#include "../headers/file-operations.hpp"
#include "../headers/superblock-operations.hpp"
#include "../headers/blocks-group-operations.hpp"
#include "../headers/inode-operations.hpp"
#include "../headers/general-constants.hpp"
#include "../headers/directory-operations.hpp"
#include "../headers/util-operations.hpp"

Ext2FileManager::Ext2FileManager(FILE *ext2_image)
{
  this->ext2_image = ext2_image;
  this->superblock = read_ext2_superblock(this->ext2_image);
  this->blocks_group_descriptor = read_ext2_blocks_group_descriptor(this->ext2_image, block_group_descriptor_address(0));
  Ext2_Inode *first_inode = read_ext2_inode(this->ext2_image, this->blocks_group_descriptor, 2);
  vector<Ext2_Directory> directories = read_ext2_directories(this->ext2_image, first_inode);
  this->history_navigation.push_back(directories.at(0));
}

bool Ext2FileManager::cd(const char *directory_name)
{

  if (!std::strcmp(directory_name, "."))
  {
    print_directory(this->history_navigation.back());
    return true;
  }
  if (!std::strcmp(directory_name, "..") && this->history_navigation.size() == 1)
  {
    std::cout << std::string(BOLD) << "no directories to go back" << std::string(DEFAULT) << std::endl;
    return true;
  }
  if (!std::strcmp(directory_name, ".."))
  {
    this->history_navigation.pop_back();
    Ext2_Directory actual_directory = this->history_navigation.back();
    unsigned int directory_inode_block_group = block_group_from_inode(this->superblock, actual_directory.inode);
    this->blocks_group_descriptor = read_ext2_blocks_group_descriptor(this->ext2_image, block_group_descriptor_address(directory_inode_block_group));
    print_directory(this->history_navigation.back());
    return true;
  }

  Ext2_Directory actual_directory = this->history_navigation.at(this->history_navigation.size() - 1);
  Ext2_Inode *actual_inode = read_ext2_inode(this->ext2_image, this->blocks_group_descriptor, inode_order_on_block_group(this->superblock, actual_directory.inode));
  Ext2_Directory *directory = search_directory(this->ext2_image, actual_inode, directory_name);

  if (!directory)
    return false;

  unsigned int directory_inode_block_group = block_group_from_inode(this->superblock, directory->inode);
  this->blocks_group_descriptor = read_ext2_blocks_group_descriptor(this->ext2_image, block_group_descriptor_address(directory_inode_block_group));

  this->history_navigation.push_back(*directory);
  print_directory(this->history_navigation.back());

  return true;
}

void Ext2FileManager::ls()
{
  Ext2_Directory actual_directory = this->history_navigation.at(this->history_navigation.size() - 1);
  Ext2_Inode *actual_inode = read_ext2_inode(this->ext2_image, this->blocks_group_descriptor, inode_order_on_block_group(this->superblock, actual_directory.inode));
  vector<Ext2_Directory> directories = read_ext2_directories(this->ext2_image, actual_inode);
  print_directories(directories);
}

void Ext2FileManager::touch(const char *directory_name, unsigned int directory_name_length)
{
  Ext2_Directory actual_directory = this->history_navigation.back();
  Ext2_Inode *actual_inode = read_ext2_inode(this->ext2_image, this->blocks_group_descriptor, inode_order_on_block_group(this->superblock, actual_directory.inode));
  vector<Ext2_Directory> directories = read_ext2_directories(this->ext2_image, actual_inode);

  Ext2_Directory last_old_directory_to_update = directories.back();
  last_old_directory_to_update.rec_len = 8 + bytes_to_4_bytes_groups_length(last_old_directory_to_update.name_len);

  uint32_t new_directory_init_position_on_block = 0;

  for (vector<Ext2_Directory>::iterator it = directories.begin(); it != directories.end() - 1; it++)
  {
    new_directory_init_position_on_block += (*it).rec_len;
  }

  new_directory_init_position_on_block += last_old_directory_to_update.rec_len;

  Ext2_Directory *new_directory = (Ext2_Directory *)malloc(sizeof(Ext2_Directory));

  unsigned int directory_name_length_as_4_bytes_group = bytes_to_4_bytes_groups_length(directory_name_length);

  new_directory->file_type = 1;
  new_directory->inode = 12;
  new_directory->name_len = directory_name_length;
  new_directory->rec_len = 1024 - new_directory_init_position_on_block;

  memcpy(new_directory->name, directory_name, directory_name_length);

  uint32_t new_directory_init_position = new_directory_init_position_on_block + BLOCK_OFFSET(actual_inode->i_block[0]);

  fseek(this->ext2_image, new_directory_init_position - last_old_directory_to_update.rec_len, SEEK_SET);
  fwrite(&last_old_directory_to_update, 1, last_old_directory_to_update.rec_len, this->ext2_image);

  fseek(this->ext2_image, new_directory_init_position, SEEK_SET);
  fwrite(new_directory, 1, new_directory->rec_len, this->ext2_image);
}

bool Ext2FileManager::rename(const char *directory_name, const char *new_directory_name, unsigned int new_directory_name_length)
{
  Ext2_Directory actual_directory = this->history_navigation.at(this->history_navigation.size() - 1);
  Ext2_Inode *actual_inode = read_ext2_inode(this->ext2_image, this->blocks_group_descriptor, inode_order_on_block_group(this->superblock, actual_directory.inode));

  unsigned int directory_init_position_on_block = 0;
  Ext2_Directory *directory = search_directory_and_position(this->ext2_image, actual_inode, directory_name, &directory_init_position_on_block);
  
  if (!directory)
    return false;

  vector<Ext2_Directory> directories = read_ext2_directories(this->ext2_image, actual_inode);
  Ext2_Directory last_intern_directory = directories.back();

  if((bytes_to_4_bytes_groups_length(new_directory_name_length) - directory->name_len) > last_intern_directory.rec_len)
    throw "there is not enough space in the block to store the new name";

  /* caso especial em que o diretório que vai ter o nome alterado é o ultimo diretório do bloco */
  if(directory_init_position_on_block == (1024 - directory->rec_len)){
    memcpy(directory->name, new_directory_name, new_directory_name_length);
    directory->rec_len = directory->rec_len - (bytes_to_4_bytes_groups_length(new_directory_name_length) - bytes_to_4_bytes_groups_length(directory->name_len));
    directory->name_len = new_directory_name_length;
    fseek(ext2_image, directory_init_position_on_block + BLOCK_OFFSET(actual_inode->i_block[0]), SEEK_SET);
    fwrite(directory, 1, 8 + directory->name_len, this->ext2_image);
    return true;
  }

  unsigned int prox_directory_init_position_on_block = directory_init_position_on_block + directory->rec_len;
  uint32_t init_position_to_shift =  prox_directory_init_position_on_block + BLOCK_OFFSET(actual_inode->i_block[0]);
  unsigned int bytes_to_shift = (1024 - last_intern_directory.rec_len) + (8 + last_intern_directory.name_len) - prox_directory_init_position_on_block;
  unsigned int offset = bytes_to_4_bytes_groups_length(new_directory_name_length) - bytes_to_4_bytes_groups_length(directory->name_len);


  fseek(this->ext2_image, 1024 - (last_intern_directory.rec_len) + BLOCK_OFFSET(actual_inode->i_block[0]), SEEK_SET);
  last_intern_directory.rec_len -= offset;
  fwrite(&last_intern_directory, 1, (8 + bytes_to_4_bytes_groups_length(last_intern_directory.name_len)), this->ext2_image);

  shift_bytes(this->ext2_image, prox_directory_init_position_on_block + BLOCK_OFFSET(actual_inode->i_block[0]), bytes_to_shift, offset);

  memcpy(directory->name, new_directory_name, new_directory_name_length);
  directory->name_len = new_directory_name_length;
  directory->rec_len = 8 + bytes_to_4_bytes_groups_length(new_directory_name_length);

  fseek(this->ext2_image, directory_init_position_on_block + BLOCK_OFFSET(actual_inode->i_block[0]), SEEK_SET);
  fwrite(directory, 1, directory->rec_len, this->ext2_image);

  return true;
}



bool Ext2FileManager::cat(const char *directory_name)
{

  Ext2_Directory actual_directory = this->history_navigation.at(this->history_navigation.size() - 1);
  Ext2_Inode *actual_inode = read_ext2_inode(this->ext2_image, this->blocks_group_descriptor, inode_order_on_block_group(this->superblock, actual_directory.inode));

  Ext2_Directory *directory = search_directory(this->ext2_image, actual_inode, directory_name);
  if (!directory)
    return false;

  unsigned int directory_inode_block_group = block_group_from_inode(this->superblock, directory->inode);
  cout << "cat:: reading " << directory_inode_block_group << endl;
  Ext2_Blocks_Group_Descriptor *bgd_of_inode = read_ext2_blocks_group_descriptor(this->ext2_image, block_group_descriptor_address(directory_inode_block_group));

  Ext2_Inode *directory_inode = read_ext2_inode(this->ext2_image, bgd_of_inode, inode_order_on_block_group(this->superblock, directory->inode));
  print_inode_blocks_content(this->ext2_image, directory_inode);
}

void Ext2FileManager::info_superblock()
{
  print_superblock(this->superblock);
}

void Ext2FileManager::info_blocks_group_descriptor()
{
  print_ext2_blocks_group_descriptor(this->blocks_group_descriptor);
}

void Ext2FileManager::info_inode(unsigned int inode)
{
  unsigned int inode_block_group = block_group_from_inode(this->superblock, inode);
  unsigned int bgd = block_group_from_inode(this->superblock, inode);
  Ext2_Blocks_Group_Descriptor *blocks_group_descriptor_of_inode = read_ext2_blocks_group_descriptor(this->ext2_image, block_group_descriptor_address(inode_block_group));
  print_ext2_blocks_group_descriptor(blocks_group_descriptor_of_inode);
  Ext2_Inode *found_inode = read_ext2_inode(this->ext2_image, blocks_group_descriptor_of_inode, inode_order_on_block_group(this->superblock, inode));
  print_ext2_inode(found_inode);
}

std::string Ext2FileManager::pwd()
{
  std::string str;
  for (vector<Ext2_Directory>::iterator it = this->history_navigation.begin(); it != this->history_navigation.end(); it++)
  {
    if (std::strcmp((*it).name, "."))
      str = str.append((*it).name);
    str = str.append("/");
  }

  return str;
}

bool Ext2FileManager::attr(const char *directory_name){
  Ext2_Directory actual_directory = this->history_navigation.at(this->history_navigation.size() - 1);
  Ext2_Inode *actual_inode = read_ext2_inode(this->ext2_image, this->blocks_group_descriptor, inode_order_on_block_group(this->superblock, actual_directory.inode));

  Ext2_Directory *directory = search_directory(this->ext2_image, actual_inode, directory_name);
  if (!directory)
    return false;

  unsigned int directory_inode_block_group = block_group_from_inode(this->superblock, directory->inode);
  Ext2_Blocks_Group_Descriptor *bgd_of_inode = read_ext2_blocks_group_descriptor(this->ext2_image, block_group_descriptor_address(directory_inode_block_group));
  Ext2_Inode *directory_inode = read_ext2_inode(this->ext2_image, bgd_of_inode, inode_order_on_block_group(this->superblock, directory->inode));
  
  std::string permission = permission_i_mode(directory_inode->i_mode); 

  cout << "permissões\t" << "uid\t" << "gid\t" << "tamanho\t" << "modificado em\t" << endl;
  cout << permission << "\t" << (unsigned)directory_inode->i_uid << "\t" 
       << (unsigned)directory_inode->i_gid << "\t" << (unsigned)directory_inode->i_size 
       << "\t"; print_time_from_unix((unsigned)directory_inode->i_mtime);

}

void Ext2FileManager::info(){

  uint32_t g_count = this->superblock->s_blocks_per_group/BLOCK_SIZE;
  uint32_t i_table = this->superblock->s_inodes_per_group/g_count;


  cout << "Volume name.....: " << this->superblock->s_volume_name << endl;
  cout << "Image size......: " << this->superblock->s_volume_name << " bytes" << endl; //alterar
  cout << "Free space......: " << this->superblock->s_volume_name << endl; //alterar
  cout << "Free inodes.....: " << this->superblock->s_free_inodes_count << endl;
  cout << "Free blocks.....: " << this->superblock->s_free_blocks_count << endl;
  cout << "Blocks size.....: " << BLOCK_SIZE << " bytes" << endl;
  cout << "Inode size......: " << this->superblock->s_inode_size << " bytes" << endl;
  cout << "Group count.....: " << g_count << endl;
  cout << "Group size......: " << this->superblock->s_blocks_per_group << " blocks" << endl;
  cout << "Group inodes....: " << this->superblock->s_inodes_per_group << " inodes" << endl;
  cout << "Inodetable size.: " << i_table << " blocks" << endl;
}