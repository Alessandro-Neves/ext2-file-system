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

  /* setando o bitmap do novo inode */
  // this->set_bit_of_inode_bitmap(12292, true);

  /* setando os valores iniciais do novo inode na tabela de inodes */
  uint32_t bgd_order = block_group_from_inode(this->superblock, 12292);
  Ext2_Blocks_Group_Descriptor *blocks_group_descriptor_of_inode = read_ext2_blocks_group_descriptor(this->ext2_image, block_group_descriptor_address(bgd_order));
  uint32_t inode_order = inode_order_on_block_group(this->superblock, 12292);
  write_ext2_inode(this->ext2_image, blocks_group_descriptor_of_inode, inode_order);

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
  new_directory->inode = 12292;
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
  // print_ext2_blocks_group_descriptor(blocks_group_descriptor_of_inode);
  cout << string(RED) << block_group_from_inode(this->superblock, inode) << " " <<  inode_order_on_block_group(this->superblock, inode) << string(DEFAULT) << endl;
  Ext2_Inode *found_inode = read_ext2_inode(this->ext2_image, blocks_group_descriptor_of_inode, inode_order_on_block_group(this->superblock, inode));
  print_ext2_inode(found_inode);
}

void Ext2FileManager::print_block_bitmap(unsigned int inode){  
  uint32_t bitmap_absolut_position = BLOCK_OFFSET(this->blocks_group_descriptor->bg_inode_bitmap);
  cout << string(RED) << bitmap_absolut_position << string(DEFAULT) << std::endl;
  unsigned int blocks_per_group = this->superblock->s_blocks_per_group;

  unsigned int bitsmap_size_as_bytes = ( blocks_per_group / 8 );

  char* bitmap = (char*)malloc(sizeof(char) * bitsmap_size_as_bytes);

  fseek(this->ext2_image, bitmap_absolut_position, SEEK_SET);
  fread(bitmap, 1, bitsmap_size_as_bytes, this->ext2_image);

  for(int index = 0; index < bitsmap_size_as_bytes; index++)
    bitmap[index] = reverse_bits(bitmap[index]);

  for(int index = 0; index < bitsmap_size_as_bytes; index++){
    if(!(index % 4) && index >0) std::cout << std::endl;
    std::cout << std::bitset<8>(bitmap[index]) << " ";
  }
}

void Ext2FileManager::print_byte_on_bitmap_of_inode(unsigned int inode) {
  uint32_t bgd_order = block_group_from_inode(this->superblock, inode);
  Ext2_Blocks_Group_Descriptor *blocks_group_descriptor_of_inode = read_ext2_blocks_group_descriptor(this->ext2_image, block_group_descriptor_address(bgd_order));
  uint32_t inode_order = inode_order_on_block_group(this->superblock, inode);

  char byte_of_bitmap = get_byte_of_bitmap(blocks_group_descriptor_of_inode, inode_order / 8, this->ext2_image);
  unsigned int index_of_bit = ((inode - (this->superblock->s_inodes_per_group * bgd_order)) - 1) % 8;

  std::cout << string(YELLOW) << index_of_bit << string(DEFAULT) << " " << std::bitset<8>(reverse_bits(byte_of_bitmap)) << std::endl;
}

void Ext2FileManager::set_bit_of_inode_bitmap(unsigned int inode, bool value) {
  uint32_t bgd_order = block_group_from_inode(this->superblock, inode);
  Ext2_Blocks_Group_Descriptor *blocks_group_descriptor_of_inode = read_ext2_blocks_group_descriptor(this->ext2_image, block_group_descriptor_address(bgd_order));
  uint32_t inode_order = inode_order_on_block_group(this->superblock, inode);

  char byte_of_bitmap = get_byte_of_bitmap(blocks_group_descriptor_of_inode, inode_order / 8, this->ext2_image);

  unsigned int index_of_bit = ((inode - (this->superblock->s_inodes_per_group * bgd_order)) - 1) % 8;

  set_byte_on_bitmap(set_bit(byte_of_bitmap, index_of_bit, value), blocks_group_descriptor_of_inode, inode_order / 8, this->ext2_image);
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
