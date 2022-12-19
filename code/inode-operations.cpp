#include <iostream>
#include <fstream>
#include <cstdbool>
#include <ctime>

#include "../headers/inode-operations.hpp"
#include "../headers/colors.hpp"
#include "../headers/util-operations.hpp"
#include "../headers/blocks-group-operations.hpp"
#include "../headers/error.hpp"

using namespace std;

void print_array(uint32_t *array, int size)
{
  for (int i = 0; i < size; i++)
    cout << "pointer[" << i << "]:  " << array[i] << endl;
}

/**
 * le o nº inode do grupo de bloco
 */
Ext2_Inode *read_ext2_inode(FILE *ext2_image, Ext2_Blocks_Group_Descriptor *block_group_descriptor, unsigned int inode_order)
{

  Ext2_Inode *inode = (Ext2_Inode *)malloc(sizeof(Ext2_Inode));

  int inode_position = BLOCK_OFFSET(block_group_descriptor->bg_inode_table) + ((inode_order - 1) * sizeof(struct ext2_inode));

  fseek(ext2_image, inode_position, SEEK_SET);

  fread(inode, 1, sizeof(Ext2_Inode), ext2_image);
  return inode;
}

void write_ext2_inode(FILE *ext2_image, Ext2_Blocks_Group_Descriptor *block_group_descriptor, unsigned int inode_order)
{

  Ext2_Inode *inode = create_default_inode();

  int inode_position = BLOCK_OFFSET(block_group_descriptor->bg_inode_table) + ((inode_order - 1) * sizeof(struct ext2_inode));

  fseek(ext2_image, inode_position, SEEK_SET);

  fwrite(inode, 1, sizeof(Ext2_Inode), ext2_image);
}

/**
 * retorna a ordem do inode dentro do descritor de grupo
 * Obs: Para o primeiro inode do descritor retorna 1
 */
unsigned int inode_order_on_block_group(Ext2_Superblock *superblock, uint32_t inode)
{
  return (unsigned int)inode % superblock->s_inodes_per_group;
}

void print_ext2_inode(Ext2_Inode *inode)
{
  cout << "i_mode:  " << "0x" << std::hex << (unsigned)inode->i_mode << std::dec << endl;
  cout << "user_id:  " << (unsigned)inode->i_uid << endl;
  cout << "lower 32-bit file size:  " << (unsigned)inode->i_size << endl;
  cout << "access time:   " << (unsigned)inode->i_atime << endl;
  cout << "creation time:  " << (unsigned)inode->i_ctime << endl;
  cout << "modification time:  " << (unsigned)inode->i_mtime << endl;
  cout << "deletion time:  " << (unsigned)inode->i_dtime << endl;
  cout << "group id:  " << (unsigned)inode->i_gid << endl;
  cout << "link count inode:  " << (unsigned)inode->i_links_count << endl;
  cout << "512-bytes blocks:  " << (unsigned)inode->i_blocks << endl;
  cout << "ext2_flags:  " << (unsigned)inode->i_flags << endl;
  cout << "reserved (Linux):  " << (unsigned)inode->osd1 << endl;
  print_array(inode->i_block, 15);
  cout << "file version (nfs):  " << (unsigned)inode->i_generation << endl;
  cout << "block number extended attributes:  " << (unsigned)inode->i_file_acl << endl;
  cout << "higher 32-bit file size:  " << (unsigned)inode->i_dir_acl << endl;
  cout << "location file fragment:  " << (unsigned)inode->i_faddr << endl;
}

static unsigned int last_position_of_content_on_block(unsigned int bytes_to_read)
{
  return bytes_to_read >= BLOCK_SIZE ? BLOCK_SIZE : bytes_to_read;
}

bool copy_array_of_blocks(FILE *ext2_image, uint32_t *indexes, int qtd_indexes, unsigned int *bytes_to_read, unsigned int *blocks_read, ofstream *destiny, bool binary)
{
  char *content = (char *)malloc(sizeof(char) * BLOCK_SIZE);
  int position;

  bool exit = false;

  for (int i = 0; i < qtd_indexes; i++)
  {
    if ((*bytes_to_read) <= BLOCK_SIZE)
      exit = true;

    position = BLOCK_OFFSET(indexes[i]);
    fseek(ext2_image, position, SEEK_SET);
    fread(content, 1, BLOCK_SIZE, ext2_image);

    if (binary)
      (*destiny).write(reinterpret_cast<char *>(content), BLOCK_SIZE);
    else
      (*destiny) << content;

    if (exit)
      return false;

    (*bytes_to_read) -= 1024;
    (*blocks_read)++;
  }

  return true;
}

void copy_inode_blocks_content(FILE *ext2_image, Ext2_Inode *inode, const char *file_name, const char *path_destiny)
{
  unsigned int bytes_to_read = inode->i_size;
  unsigned int blocks_read = 0;
  bool exit = false;
  int position;

  uint32_t *indexes_level_1 = (uint32_t *)malloc(sizeof(uint32_t) * 256);
  uint32_t *indexes_level_2 = (uint32_t *)malloc(sizeof(uint32_t) * 256);
  uint32_t *indexes_level_3 = (uint32_t *)malloc(sizeof(uint32_t) * 256);

  if (!directory_exists(path_destiny))
    throw new FileManagerInfo("destination directory not exists.");

  std::string str_path = std::string(path_destiny) + std::string("/") + std::string(file_name);
  bool binary = (str_path.find(".txt") == std::string::npos) ? true : false;

  ofstream host_file(str_path.c_str(), std::ios::out | std::ios::binary | std::ios::app);

  if (!host_file.is_open())
    return;

  /* impressão niveis de acesso direto */
  if (!copy_array_of_blocks(ext2_image, inode->i_block, 12, &bytes_to_read, &blocks_read, &host_file, binary))
  {
    host_file.close();
    return;
  }

  /* impressão niveis simples de indexes */
  position = BLOCK_OFFSET((inode->i_block)[12]);
  fseek(ext2_image, position, SEEK_SET);
  fread(indexes_level_1, 1, BLOCK_SIZE, ext2_image);
  if (!copy_array_of_blocks(ext2_image, indexes_level_1, 256, &bytes_to_read, &blocks_read, &host_file, binary))
  {
    host_file.close();
    return;
  }

  /* impressão niveis duplos de indexes */
  position = BLOCK_OFFSET((inode->i_block)[13]);
  fseek(ext2_image, position, SEEK_SET);
  fread(indexes_level_2, 1, BLOCK_SIZE, ext2_image);

  for (int i = 0; i < 256; i++)
  {
    position = BLOCK_OFFSET(indexes_level_2[i]);
    fseek(ext2_image, position, SEEK_SET);
    fread(indexes_level_1, 1, BLOCK_SIZE, ext2_image);
    if (!copy_array_of_blocks(ext2_image, indexes_level_1, 256, &bytes_to_read, &blocks_read, &host_file, binary))
    {
      host_file.close();
      return;
    }
  }

  /* impressão niveis triplos de indexes */
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
      if (!copy_array_of_blocks(ext2_image, indexes_level_1, 256, &bytes_to_read, &blocks_read, &host_file, binary))
      {
        host_file.close();
        return;
      }
    }
  }
}

bool _print_array_of_blocks(FILE *ext2_image, uint32_t *indexes, int qtd_indexes, unsigned int *bytes_to_read, unsigned int *blocks_read, unsigned int total)
{
  char *content = (char *)malloc(sizeof(char) * (BLOCK_SIZE + 1));
  int position;

  bool exit = false;

  // system("clear");
  // cout << string(BLUE) << "[ " << ((*blocks_read) / 10) + 1 << " of " << (*bytes_to_read) / ((BLOCK_SIZE * 10)) + 1 << " ]" << string(DEFAULT) << endl
  //      << endl;

  for (int i = 0; i < qtd_indexes; i++)
  {
    if ((*bytes_to_read) <= BLOCK_SIZE)
      exit = true;

    // if ((*blocks_read) % 10 == 0 && (*blocks_read) > 0)
    // {
    //   cout << string(GREEN) << endl
    //        << "[ press ANY key to read more, ESQ to exit ]\t" << string(DEFAULT);
    //   system("stty raw");
    //   if (getchar() == 27)
    //   {
    //     system("stty cooked");
    //     system("clear");
    //     return false;
    //   }
    //   system("stty cooked");
    //   system("clear");
    //   cout << string(BLUE) << "[ " << (*blocks_read / 10) + 1 << " of " << total / ((BLOCK_SIZE * 10)) + 1 << " ]" << string(DEFAULT) << endl
    //        << endl;
    // }

    position = BLOCK_OFFSET(indexes[i]);
    fseek(ext2_image, position, SEEK_SET);
    fread(content, 1, BLOCK_SIZE, ext2_image);

    content[last_position_of_content_on_block(*bytes_to_read)] = '\0';

    cout << content;

    if (exit)
    {
      // cout << string(YELLOW) << "[ end of file, press any key to exit ]\t" << string(DEFAULT);
      // cin.get();
      // system("clear");
      return false;
    };
    (*bytes_to_read) -= 1024;
    (*blocks_read)++;
  }

  return true;
}

void print_inode_blocks_content(FILE *ext2_image, Ext2_Inode *inode)
{
  unsigned int bytes_to_read = inode->i_size;
  unsigned int blocks_read = 0;
  bool exit = false;
  int position;

  uint32_t *indexes_level_1 = (uint32_t *)malloc(sizeof(uint32_t) * 256);
  uint32_t *indexes_level_2 = (uint32_t *)malloc(sizeof(uint32_t) * 256);
  uint32_t *indexes_level_3 = (uint32_t *)malloc(sizeof(uint32_t) * 256);

  /* impressão niveis de acesso direto */
  if (!_print_array_of_blocks(ext2_image, inode->i_block, 12, &bytes_to_read, &blocks_read, inode->i_size))
    return;

  /* impressão niveis simples de indexes */
  position = BLOCK_OFFSET((inode->i_block)[12]);
  fseek(ext2_image, position, SEEK_SET);
  fread(indexes_level_1, 1, BLOCK_SIZE, ext2_image);
  if (!_print_array_of_blocks(ext2_image, indexes_level_1, 256, &bytes_to_read, &blocks_read, inode->i_size))
    return;

  /* impressão niveis duplos de indexes */
  position = BLOCK_OFFSET((inode->i_block)[13]);
  fseek(ext2_image, position, SEEK_SET);
  fread(indexes_level_2, 1, BLOCK_SIZE, ext2_image);

  for (int i = 0; i < 256; i++)
  {
    position = BLOCK_OFFSET(indexes_level_2[i]);
    fseek(ext2_image, position, SEEK_SET);
    fread(indexes_level_1, 1, BLOCK_SIZE, ext2_image);
    if (!_print_array_of_blocks(ext2_image, indexes_level_1, 256, &bytes_to_read, &blocks_read, inode->i_size))
      return;
  }

  /* impressão niveis triplos de indexes */
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
      if (!_print_array_of_blocks(ext2_image, indexes_level_1, 256, &bytes_to_read, &blocks_read, inode->i_size))
        return;
    }
  }
}

Ext2_Inode *create_default_inode()
{
  Ext2_Inode *inode = (Ext2_Inode *)calloc(1, sizeof(Ext2_Inode));

  inode->i_mode = (uint16_t)0x81A4;

  inode->i_gid = 0;
  inode->i_uid = 0;

  inode->i_ctime = std::time(nullptr);
  inode->i_atime = std::time(nullptr);
  inode->i_dtime = 0;

  inode->i_size = 0;
  inode->i_links_count = 1;
  inode->i_flags = 0;
  inode->i_blocks = 0;

  inode->i_generation = 3079266834;
  inode->i_file_acl = 0;
  inode->i_dir_acl = 0;
  inode->i_faddr = 0;

  return inode;
}

char get_byte_of_inode_bitmap(Ext2_Blocks_Group_Descriptor *bgd, uint32_t byte_index, FILE *ext2_image)
{
  uint32_t absolut_bitmap_position = BLOCK_OFFSET(bgd->bg_inode_bitmap);
  uint32_t absolut_byte_position = absolut_bitmap_position + byte_index;

  char byte = 0;

  fseek(ext2_image, absolut_byte_position, SEEK_SET);
  fread(&byte, 1, 1, ext2_image);
  return byte;
}

void set_byte_on_inode_bitmap(char byte, Ext2_Blocks_Group_Descriptor *bgd, uint32_t byte_order, FILE *ext2_image)
{
  uint32_t absolut_bitmap_position = BLOCK_OFFSET(bgd->bg_inode_bitmap);
  uint32_t absolut_byte_position = absolut_bitmap_position + byte_order;

  fseek(ext2_image, absolut_byte_position, SEEK_SET);
  fwrite(&byte, 1, 1, ext2_image);
}

uint32_t find_free_inode(Ext2_Superblock *superblock, FILE *ext2_image)
{

  uint32_t absolute_byte_bitmap = 0;
  for (int index = 0; index < 8; index++)
  {
    uint32_t bgd_absolute_address = block_group_descriptor_address(index);
    Ext2_Blocks_Group_Descriptor *bgd = read_ext2_blocks_group_descriptor(ext2_image, bgd_absolute_address);
    for (int byte_index = 0; byte_index < 256; byte_index++)
    {
      char byte = get_byte_of_inode_bitmap(bgd, byte_index, ext2_image);
      if (byte != (char)0b11111111)
      {
        int index_of_first_zero_bit = find_first_zero_bit(byte);
        if (index_of_first_zero_bit != -1)
          return (absolute_byte_bitmap * 8) + index_of_first_zero_bit + 1; // +1: utilizado porque os inodes omeçam a partir de 1
        else
          throw new Error("inode-operations::find_free_inode::0");
      }
      else
        absolute_byte_bitmap++;
    }
  }

  return -1;
}

std::string permission_i_mode(uint32_t i_mode)
{
  std::string str;

  if (i_mode & (0x4000))
    str = str.append("d");

  else if (i_mode & (0x8000))
    str = str.append("f");
  else
    str = str.append("-");

  // user
  if (i_mode & (0x0100))
    str = str.append("r");
  else
    str = str.append("-");

  if (i_mode & (0x0080))
    str = str.append("w");
  else
    str = str.append("-");

  if (i_mode & (0x0040))
    str = str.append("x");
  else
    str = str.append("-");

  // group
  if (i_mode & (0x0020))
    str = str.append("r");
  else
    str = str.append("-");

  if (i_mode & (0x0010))
    str = str.append("w");
  else
    str = str.append("-");

  if (i_mode & (0x0008))
    str = str.append("r");
  else
    str = str.append("-");

  // other
  if (i_mode & (0x0004))
    str = str.append("r");
  else
    str = str.append("-");

  if (i_mode & (0x0002))
    str = str.append("w");
  else
    str = str.append("-");

  if (i_mode & (0x0001))
    str = str.append("r");
  else
    str = str.append("-");

  return str;
}