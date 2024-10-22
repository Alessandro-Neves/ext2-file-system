#include "../headers/ext2-file-manager.hpp"
#include "../headers/ext2.hpp"
#include "../headers/file-operations.hpp"
#include "../headers/superblock-operations.hpp"
#include "../headers/blocks-group-operations.hpp"
#include "../headers/inode-operations.hpp"
#include "../headers/general-constants.hpp"
#include "../headers/directory-operations.hpp"
#include "../headers/util-operations.hpp"
#include "../headers/block-operations.hpp"
#include "../headers/error.hpp"

/* construtor do gerenciador/manipulador do sistema de arquivos, 
realiza a inicialização e leitura de informações importantes como o arquivo da imagem 
e a leitura do diretório principal */
Ext2FileManager::Ext2FileManager(FILE *ext2_image)
{
  this->ext2_image = ext2_image;
  this->superblock = read_ext2_superblock(this->ext2_image);
  this->blocks_group_descriptor = read_ext2_blocks_group_descriptor(this->ext2_image, block_group_descriptor_address(0));
  Ext2_Inode *first_inode = read_ext2_inode(this->ext2_image, this->blocks_group_descriptor, 2);
  vector<Ext2_Directory> directories = read_ext2_directories(this->ext2_image, first_inode);
  this->history_navigation.push_back(directories.at(0));
}

/* realiza a navegação para um determinado diretório, verificando antes a existência do mesmo e a validade da operação */
void Ext2FileManager::cd(const char *directory_name)
{

  if (!std::strcmp(directory_name, "."))
  {
    print_directory(this->history_navigation.back());
    return;
  }
  if (!std::strcmp(directory_name, "..") && this->history_navigation.size() == 1)
    throw new FileManagerInfo("no directories to go back.");
  if (!std::strcmp(directory_name, ".."))
  {
    this->history_navigation.pop_back();
    Ext2_Directory actual_directory = this->history_navigation.back();
    unsigned int directory_inode_block_group = block_group_from_inode(this->superblock, actual_directory.inode);
    this->blocks_group_descriptor = read_ext2_blocks_group_descriptor(this->ext2_image, block_group_descriptor_address(directory_inode_block_group));
    print_directory(this->history_navigation.back());
    return;
  }

  Ext2_Directory actual_directory = this->history_navigation.at(this->history_navigation.size() - 1);
  Ext2_Inode *actual_inode = read_ext2_inode(this->ext2_image, this->blocks_group_descriptor, inode_order_on_block_group(this->superblock, actual_directory.inode));
  Ext2_Directory *directory = search_directory(this->ext2_image, actual_inode, directory_name);

  if(!directory) throw new FileManagerInfo("directory not found.");
  if (directory->file_type != 2)  throw new FileManagerInfo("it's not a directory.");

  unsigned int directory_inode_block_group = block_group_from_inode(this->superblock, directory->inode);
  this->blocks_group_descriptor = read_ext2_blocks_group_descriptor(this->ext2_image, block_group_descriptor_address(directory_inode_block_group));

  this->history_navigation.push_back(*directory);
  print_directory(this->history_navigation.back());
}

/* realiza a listagem dos diretórios contidos no diretório referenciado atualmente pela navegação */
void Ext2FileManager::ls()
{
  Ext2_Directory actual_directory = this->history_navigation.at(this->history_navigation.size() - 1);
  Ext2_Inode *actual_inode = read_ext2_inode(this->ext2_image, this->blocks_group_descriptor, inode_order_on_block_group(this->superblock, actual_directory.inode));
  vector<Ext2_Directory> directories = read_ext2_directories(this->ext2_image, actual_inode);
  print_directories(directories);
}

/* realiza a criação e inicialização de um novo diretório/arquivos no sistema */
void Ext2FileManager::touch(const char *directory_name, unsigned int directory_name_length)
{

  Ext2_Directory actual_directory = this->history_navigation.back();
  Ext2_Inode *actual_inode = read_ext2_inode(this->ext2_image, this->blocks_group_descriptor, inode_order_on_block_group(this->superblock, actual_directory.inode));

  Ext2_Directory *directory = search_directory(this->ext2_image, actual_inode, directory_name);
  if (directory)  throw new FileManagerInfo("file alread exists.");

  vector<Ext2_Directory> directories = read_ext2_directories(this->ext2_image, actual_inode);

  /* setando o bitmap do novo inode */

  uint32_t free_inode_index = find_free_inode(this->superblock, this->ext2_image);

  /* Seta o bitmap do inode e atualiza as informação do block group descriptor */
  this->set_bit_of_inode_bitmap(free_inode_index, true);

  /* setando os valores iniciais do novo inode na tabela de inodes */
  uint32_t bgd_order = block_group_from_inode(this->superblock, free_inode_index);
  Ext2_Blocks_Group_Descriptor *blocks_group_descriptor_of_inode = read_ext2_blocks_group_descriptor(this->ext2_image, block_group_descriptor_address(bgd_order));
  uint32_t inode_order = inode_order_on_block_group(this->superblock, free_inode_index);

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
  new_directory->inode = free_inode_index;
  new_directory->name_len = directory_name_length;
  new_directory->rec_len = 1024 - new_directory_init_position_on_block;

  memcpy(new_directory->name, directory_name, directory_name_length);

  uint32_t new_directory_init_position = new_directory_init_position_on_block + BLOCK_OFFSET(actual_inode->i_block[0]);

  fseek(this->ext2_image, new_directory_init_position - last_old_directory_to_update.rec_len, SEEK_SET);
  fwrite(&last_old_directory_to_update, 1, last_old_directory_to_update.rec_len, this->ext2_image);

  fseek(this->ext2_image, new_directory_init_position, SEEK_SET);
  fwrite(new_directory, 1, new_directory->rec_len, this->ext2_image);

  new_directory->name[directory_name_length] = '\0';
  print_directory(*new_directory);
}

/* realiza a renomeação de um determina diretório/arquivo contido dentro do diretório referênciado atualmente pela navegação */
void Ext2FileManager::rename(const char *directory_name, const char *new_directory_name, unsigned int new_directory_name_length)
{
  Ext2_Directory actual_directory = this->history_navigation.at(this->history_navigation.size() - 1);
  Ext2_Inode *actual_inode = read_ext2_inode(this->ext2_image, this->blocks_group_descriptor, inode_order_on_block_group(this->superblock, actual_directory.inode));
  vector<Ext2_Directory> directories = read_ext2_directories(this->ext2_image, actual_inode);

  int index_of_directory = 0;
  Ext2_Directory *directory_to_rename = NULL;

  for (vector<Ext2_Directory>::iterator it = directories.begin(); it != directories.end(); it++)
  {
    const char *iterator_directory_name = (const char *)(*it).name;
    if (!strcmp(directory_name, iterator_directory_name))
    {
      directory_to_rename = &(*it);
      break;
    }
    else
      index_of_directory++;
  }

  Ext2_Directory last_directory = directories.back();

  uint32_t available_space = (last_directory.rec_len - (8 + bytes_to_4_bytes_groups_length(last_directory.name_len)));

  if(!directory_to_rename) throw new FileManagerInfo("file not found.");

  if(new_directory_name_length > directory_to_rename->name_len)
    if(new_directory_name_length > available_space)
      throw new FileManagerInfo("there is not enough space in the block to store the new name.");

  if(index_of_directory == directories.size() - 1){
    memcpy(directory_to_rename->name, new_directory_name, new_directory_name_length);
    directory_to_rename->name_len = new_directory_name_length;
  }
  else {
    directories.back().rec_len += directory_to_rename->rec_len;

    memcpy(directory_to_rename->name, new_directory_name, new_directory_name_length);
    directory_to_rename->name_len = new_directory_name_length;
    directory_to_rename->rec_len = 8 + bytes_to_4_bytes_groups_length(new_directory_name_length);

    directories.back().rec_len -= directory_to_rename->rec_len;
  }

  uint32_t intern_position = 0;
  for (vector<Ext2_Directory>::iterator it = directories.begin(); it != directories.end(); it++)
  {
    fseek(this->ext2_image, BLOCK_OFFSET(actual_inode->i_block[0]) + intern_position, SEEK_SET);
    fwrite(&(*it), 1, 8 + bytes_to_4_bytes_groups_length((*it).name_len), this->ext2_image);
    intern_position += (*it).rec_len;
  }

  cout << directory_name << " renamed to " << new_directory_name << endl;
}

/* realiza a remoção de um determinado arquivo, executando antes as verificações necessárias */
void Ext2FileManager::rm(const char *directory_name, unsigned int directory_name_length, bool info)
{
  Ext2_Directory actual_directory = this->history_navigation.at(this->history_navigation.size() - 1);
  Ext2_Inode *actual_inode = read_ext2_inode(this->ext2_image, this->blocks_group_descriptor, inode_order_on_block_group(this->superblock, actual_directory.inode));
  vector<Ext2_Directory> directories = read_ext2_directories(this->ext2_image, actual_inode);

  int index_of_directory = 0;
  Ext2_Directory *directory_to_remove = NULL;

  for (vector<Ext2_Directory>::iterator it = directories.begin(); it != directories.end(); it++)
  {
    const char *iterator_directory_name = (const char *)(*it).name;
    if (!strcmp(directory_name, iterator_directory_name))
    {
      directory_to_remove = (Ext2_Directory*) calloc(1, sizeof(Ext2_Directory));
      memcpy(directory_to_remove, &(*it), sizeof(Ext2_Directory));
      break;
    }
    else
      index_of_directory++;
  }

  if(!directory_to_remove) throw new FileManagerInfo("file not found.");
  if(directory_to_remove->file_type != 1) throw new FileManagerInfo("it's not a file.");

  directories.erase(directories.begin() + index_of_directory);

  directories.back().rec_len += directory_to_remove->rec_len;

  uint32_t intern_position = 0;
  for (vector<Ext2_Directory>::iterator it = directories.begin(); it != directories.end(); it++)
  {
    fseek(this->ext2_image, BLOCK_OFFSET(actual_inode->i_block[0]) + intern_position, SEEK_SET);
    fwrite(&(*it), 1, 8 + bytes_to_4_bytes_groups_length((*it).name_len), this->ext2_image);
    intern_position += (*it).rec_len;
  }

  this->set_bit_of_inode_bitmap(directory_to_remove->inode, false);

  /* liberando os blocos utilizados pelo arquivo */
  Ext2_Inode *inode_of_dir_remove = _read_ext2_inode(directory_to_remove->inode);
  this->release_blocks_of_inode(inode_of_dir_remove);

  if(info) print_directory(*directory_to_remove);
}

/* realiza a cópia de um determinado arquivo interno para um determinado diretório do sistema do usuário */
void Ext2FileManager::copy(const char *origin_name, const char *destiny_name, bool info)
{

  Ext2_Directory actual_directory = this->history_navigation.at(this->history_navigation.size() - 1);
  Ext2_Inode *actual_inode = read_ext2_inode(this->ext2_image, this->blocks_group_descriptor, inode_order_on_block_group(this->superblock, actual_directory.inode));

  Ext2_Directory *directory = search_directory(this->ext2_image, actual_inode, origin_name);

  if(!directory) throw new FileManagerInfo("file not found.");
  if(directory->file_type != 1) throw new FileManagerInfo("it's not a file.");

  unsigned int directory_inode_block_group = block_group_from_inode(this->superblock, directory->inode);
  Ext2_Blocks_Group_Descriptor *bgd_of_inode = read_ext2_blocks_group_descriptor(this->ext2_image, block_group_descriptor_address(directory_inode_block_group));

  Ext2_Inode *directory_inode = read_ext2_inode(this->ext2_image, bgd_of_inode, inode_order_on_block_group(this->superblock, directory->inode));
  copy_inode_blocks_content(this->ext2_image, directory_inode, directory->name, destiny_name);

  if(info) cout << origin_name << " copied to " << destiny_name << endl; 
}

void Ext2FileManager::move(const char *origin_name, const char *destiny_name){
  this->copy(origin_name, destiny_name, false);
  this->rm(origin_name, string(origin_name).size(), false);
  cout << origin_name << " moved to " << destiny_name << endl;
}

/* realiza a impressão do conteudo contido em uma determinado arquivo */
void Ext2FileManager::cat(const char *directory_name)
{

  Ext2_Directory actual_directory = this->history_navigation.at(this->history_navigation.size() - 1);
  Ext2_Inode *actual_inode = read_ext2_inode(this->ext2_image, this->blocks_group_descriptor, inode_order_on_block_group(this->superblock, actual_directory.inode));

  Ext2_Directory *directory = search_directory(this->ext2_image, actual_inode, directory_name);

  if(!directory) throw new FileManagerInfo("file not found.");
  if(directory->file_type != 1) throw new FileManagerInfo("it's not a file.");

  unsigned int directory_inode_block_group = block_group_from_inode(this->superblock, directory->inode);
  Ext2_Blocks_Group_Descriptor *bgd_of_inode = read_ext2_blocks_group_descriptor(this->ext2_image, block_group_descriptor_address(directory_inode_block_group));

  Ext2_Inode *directory_inode = read_ext2_inode(this->ext2_image, bgd_of_inode, inode_order_on_block_group(this->superblock, directory->inode));
  print_inode_blocks_content(this->ext2_image, directory_inode);
}

/* realiza a impressão dos dados do superbloco */
void Ext2FileManager::info_superblock()
{
  print_superblock(this->superblock);
}

/* realiza a impressão dos dados de um determinado descritor de grupo identificado pelo index 
no bloco de descritores */
void Ext2FileManager::info_blocks_group_descriptor(int bgd_index)
{
  uint32_t bgd_absolute_address = block_group_descriptor_address(bgd_index);
  Ext2_Blocks_Group_Descriptor *bgd = read_ext2_blocks_group_descriptor(this->ext2_image, bgd_absolute_address);
  print_ext2_blocks_group_descriptor(bgd);
}

/* realiza a impressão de dados do 'inode' */
void Ext2FileManager::info_inode(unsigned int inode)
{
  unsigned int inode_block_group = block_group_from_inode(this->superblock, inode);
  unsigned int bgd = block_group_from_inode(this->superblock, inode);
  Ext2_Blocks_Group_Descriptor *blocks_group_descriptor_of_inode = read_ext2_blocks_group_descriptor(this->ext2_image, block_group_descriptor_address(inode_block_group));
  Ext2_Inode *found_inode = read_ext2_inode(this->ext2_image, blocks_group_descriptor_of_inode, inode_order_on_block_group(this->superblock, inode));
  print_ext2_inode(found_inode);
}

/* imprime o grupo de 8 bits dentro do inode bitmap ao qual o bit referente ao 'inode' pertence */
void Ext2FileManager::print_byte_on_bitmap_of_inode(unsigned int inode)
{
  uint32_t bgd_order = block_group_from_inode(this->superblock, inode);
  Ext2_Blocks_Group_Descriptor *blocks_group_descriptor_of_inode = read_ext2_blocks_group_descriptor(this->ext2_image, block_group_descriptor_address(bgd_order));
  uint32_t inode_order = inode_order_on_block_group(this->superblock, inode);

  char byte_of_bitmap = get_byte_of_inode_bitmap(blocks_group_descriptor_of_inode, (inode_order - 1) / 8, this->ext2_image);
  unsigned int index_of_bit = ((inode - (this->superblock->s_inodes_per_group * bgd_order)) - 1) % 8;

  std::cout << string(YELLOW) << index_of_bit << string(DEFAULT) << " " << std::bitset<8>(reverse_bits(byte_of_bitmap)) << std::endl;
}

/* imprime o grupo de 8 bits dentro do block bitmap ao qual o bit referente ao 'block' pertence */
void Ext2FileManager::print_byte_on_bitmap_of_block(unsigned int block)
{
  uint32_t bgd_order = block_group_from_block(this->superblock, block);
  Ext2_Blocks_Group_Descriptor *blocks_group_descriptor_of_block = read_ext2_blocks_group_descriptor(this->ext2_image, block_group_descriptor_address(bgd_order));
  uint32_t block_bit_order = bit_block_order_on_block_group(this->superblock, block);

  char byte_of_bitmap = get_byte_of_block_bitmap(blocks_group_descriptor_of_block, (block_bit_order - 1) / 8, this->ext2_image);
  unsigned int index_of_bit = ((block - (this->superblock->s_blocks_per_group * bgd_order)) - 1) % 8;

  std::cout << string(YELLOW) << index_of_bit << string(DEFAULT) << " " << std::bitset<8>(reverse_bits(byte_of_bitmap)) << std::endl;
}

/* seta o bit de um determinado 'inode' no bitmap, realizado as verificações, 
e atualizado os valores do superbloco e do descritor de grupo (inodes livres) */
void Ext2FileManager::set_bit_of_inode_bitmap(unsigned int inode, bool value)
{
  uint32_t bgd_order = block_group_from_inode(this->superblock, inode);
  uint32_t bgd_address = block_group_descriptor_address(bgd_order);
  Ext2_Blocks_Group_Descriptor *blocks_group_descriptor_of_inode = read_ext2_blocks_group_descriptor(this->ext2_image, bgd_address);
  uint32_t inode_order = inode_order_on_block_group(this->superblock, inode);

  char byte_of_bitmap = get_byte_of_inode_bitmap(blocks_group_descriptor_of_inode, (inode_order - 1) / 8, this->ext2_image);

  unsigned int index_of_bit = ((inode - (this->superblock->s_inodes_per_group * bgd_order)) - 1) % 8;

  set_byte_on_inode_bitmap(set_bit(byte_of_bitmap, index_of_bit, value), blocks_group_descriptor_of_inode, (inode_order - 1) / 8, this->ext2_image);

  /* atualiza o block group descriptor de acordo com o bloco alocado ou desalocado */
  blocks_group_descriptor_of_inode->bg_free_inodes_count += (value ? -1 : 1);
  write_ext2_blocks_group_descriptor(blocks_group_descriptor_of_inode, this->ext2_image, bgd_address);

  /* atualiza o superbloco de acordo com o inode alocado ou desalocado */
  this->superblock->s_free_inodes_count += (value ? -1 : 1);
  write_ext2_superblock(this->superblock, this->ext2_image);
}

/* seta o bit de um determinado bloco no bitmap, realizado as verificações, 
e atualizado os valores do superbloco e do descritor de grupo (blocos livres) */
void Ext2FileManager::set_bit_of_block_bitmap(unsigned int block, bool value)
{
  uint32_t bgd_order = block_group_from_block(this->superblock, block);
  uint32_t bgd_address = block_group_descriptor_address(bgd_order);
  Ext2_Blocks_Group_Descriptor *blocks_group_descriptor_of_block = read_ext2_blocks_group_descriptor(this->ext2_image, bgd_address);
  uint32_t block_bit_order = bit_block_order_on_block_group(this->superblock, block);

  char byte_of_bitmap = get_byte_of_block_bitmap(blocks_group_descriptor_of_block, (block_bit_order - 1) / 8, this->ext2_image);

  unsigned int index_of_bit = ((block - (this->superblock->s_blocks_per_group * bgd_order)) - 1) % 8;


  set_byte_on_block_bitmap(set_bit(byte_of_bitmap, index_of_bit, value), blocks_group_descriptor_of_block, (block_bit_order - 1) / 8, this->ext2_image);

  /* atualiza o block group descriptor de acordo com o bloco alocado ou desalocado */
  blocks_group_descriptor_of_block->bg_free_blocks_count += (value ? -1 : 1);
  write_ext2_blocks_group_descriptor(blocks_group_descriptor_of_block, this->ext2_image, bgd_address);

  /* atualiza o superbloco de acordo com o bloco alocado ou desalocado */
  this->superblock->s_free_blocks_count += (value ? -1 : 1);
  write_ext2_superblock(this->superblock, this->ext2_image);
}

/* constroi uma string que informa o caminha 'path' do diretório atual referênciado pela navegação */
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

/* realiza a leitura de um determinado 'inode' na imagem */
Ext2_Inode *Ext2FileManager::_read_ext2_inode(uint32_t inode)
{
  unsigned int bgd_order = block_group_from_inode(this->superblock, inode);
  unsigned int inode_order = inode_order_on_block_group(this->superblock, inode);
  Ext2_Blocks_Group_Descriptor *bgd = read_ext2_blocks_group_descriptor(this->ext2_image, block_group_descriptor_address(bgd_order));
  return read_ext2_inode(this->ext2_image, this->blocks_group_descriptor, inode_order_on_block_group(this->superblock, inode_order));
}

/* realiza a liberação 'free' dos blocos do array 'blocks' */
void Ext2FileManager::release_blocks(uint32_t *blocks, int blocks_size, uint32_t *bytes_to_remove)
{
  for (int i = 0; i < blocks_size && (*bytes_to_remove) > 0; i++)
  {
    this->set_bit_of_block_bitmap(blocks[i], false);
    if ((*bytes_to_remove) >= 1024)
      (*bytes_to_remove) -= 1024;
    else
      (*bytes_to_remove) = 0;
  }
}

/* realiza a liberação 'free' dos blocos utilizados por um determinado 'inode' */
void Ext2FileManager::release_blocks_of_inode(Ext2_Inode *inode)
{
  uint32_t bytes_to_remove = inode->i_size;
  uint32_t position;

  uint32_t *indexes_level_1 = (uint32_t *)malloc(sizeof(uint32_t) * 256);
  uint32_t *indexes_level_2 = (uint32_t *)malloc(sizeof(uint32_t) * 256);
  uint32_t *indexes_level_3 = (uint32_t *)malloc(sizeof(uint32_t) * 256);

  this->release_blocks(&inode->i_block[0], 12, &bytes_to_remove);

  /* libera blocos dos niveis simples de indexes */
  position = BLOCK_OFFSET((inode->i_block)[12]);
  fseek(ext2_image, position, SEEK_SET);
  fread(indexes_level_1, 1, BLOCK_SIZE, ext2_image);
  this->release_blocks(indexes_level_1, 256, &bytes_to_remove);

  /* libera blocos dos niveis duplos de indexes */
  position = BLOCK_OFFSET((inode->i_block)[13]);
  fseek(ext2_image, position, SEEK_SET);
  fread(indexes_level_2, 1, BLOCK_SIZE, ext2_image);
  for (int i = 0; i < 256; i++)
  {
    position = BLOCK_OFFSET(indexes_level_2[i]);
    fseek(ext2_image, position, SEEK_SET);
    fread(indexes_level_1, 1, BLOCK_SIZE, ext2_image);
    this->release_blocks(indexes_level_1, 256, &bytes_to_remove);
  }

  /* libera blocos dos niveis triplos de indexes */
  position = BLOCK_OFFSET((inode->i_block)[14]);
  fseek(ext2_image, position, SEEK_SET);
  fread(indexes_level_3, 1, BLOCK_SIZE, ext2_image);

  for (int i = 0; i < 256; i++)
  {
    position = BLOCK_OFFSET(indexes_level_3[i]);
    fseek(ext2_image, position, SEEK_SET);
    fread(indexes_level_2, 1, BLOCK_SIZE, ext2_image);

    for (int j = 0; j < 256; j++)
    {
      position = BLOCK_OFFSET(indexes_level_2[j]);
      fseek(ext2_image, position, SEEK_SET);
      fread(indexes_level_1, 1, BLOCK_SIZE, ext2_image);
      this->release_blocks(indexes_level_1, 256, &bytes_to_remove);
    }
  }
}

/* realiza a impressão de das permissões, tipo e dados básicos de um diretório/arquivo */
void Ext2FileManager::attr(const char *directory_name){
  Ext2_Directory actual_directory = this->history_navigation.at(this->history_navigation.size() - 1);
  Ext2_Inode *actual_inode = read_ext2_inode(this->ext2_image, this->blocks_group_descriptor, inode_order_on_block_group(this->superblock, actual_directory.inode));

  Ext2_Directory *directory = search_directory(this->ext2_image, actual_inode, directory_name);

  if(!directory) throw new FileManagerInfo("not found.");

  unsigned int directory_inode_block_group = block_group_from_inode(this->superblock, directory->inode);
  Ext2_Blocks_Group_Descriptor *bgd_of_inode = read_ext2_blocks_group_descriptor(this->ext2_image, block_group_descriptor_address(directory_inode_block_group));
  Ext2_Inode *directory_inode = read_ext2_inode(this->ext2_image, bgd_of_inode, inode_order_on_block_group(this->superblock, directory->inode));
  
  std::string permission = permission_i_mode(directory_inode->i_mode); 

  float size_fk = (float) directory_inode->i_size / 1024;
  float size_fm = (float) directory_inode->i_size / (1024 * 1024);

  cout << "permissions\t" << "uid\t" << "gid\t" << "size\t\t\t" << "modified on\t" << endl;
  cout << permission << "\t" << (unsigned)directory_inode->i_uid << "\t";
  cout << (unsigned)directory_inode->i_gid << "\t";
  if(directory_inode->i_size < 1024) cout << (unsigned) directory_inode->i_size << " bytes";
  else if(directory_inode->i_size < (1024 * 1024)) cout << std::setprecision(2) << size_fk << " KiB";
  else cout << std::setprecision(2) << size_fm << " MiB  ";
  cout << "\t\t"; 
  print_time_from_unix((unsigned)directory_inode->i_mtime);
}

/* realiza a impressão de dados importantes do sistema de arquivos */
void Ext2FileManager::info(){
  uint32_t g_count = this->superblock->s_blocks_per_group/BLOCK_SIZE;
  uint32_t i_table = this->superblock->s_inodes_per_group/g_count;

  cout << "Volume name.....: " << this->superblock->s_volume_name << endl;
  cout << "Image size......: " << this->superblock->s_blocks_count << " KiB" << endl;
  cout << "Free space......: " << this->superblock->s_free_blocks_count << " KiB" << endl;
  cout << "Free inodes.....: " << this->superblock->s_free_inodes_count << endl;
  cout << "Free blocks.....: " << this->superblock->s_free_blocks_count << endl;
  cout << "Blocks size.....: " << BLOCK_SIZE << " bytes" << endl;
  cout << "Inode size......: " << this->superblock->s_inode_size << " bytes" << endl;
  cout << "Group count.....: " << g_count << endl;
  cout << "Group size......: " << this->superblock->s_blocks_per_group << " blocks" << endl;
  cout << "Group inodes....: " << this->superblock->s_inodes_per_group << " inodes" << endl;
  cout << "Inodetable size.: " << i_table << " blocks" << endl;
}
