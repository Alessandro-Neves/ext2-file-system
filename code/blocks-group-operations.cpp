#include "../headers/blocks-group-operations.hpp"

Ext2_Blocks_Group_Descriptor *read_ext2_blocks_group_descriptor(FILE *ext2_image, uint32_t at_position)
{

  /* aloca espaço na memória suficiente para armazenar a estrutura do Ext2_Blocks_Group_Descriptor */
  Ext2_Blocks_Group_Descriptor *blocks_group_descriptor = (Ext2_Blocks_Group_Descriptor *)malloc(sizeof(Ext2_Blocks_Group_Descriptor));

  /* posicionar o ponteiro no início do superbloco (bite 2048) */
  fseek(ext2_image, at_position, SEEK_SET);

  /* copiar 288 unidade de 1byte a partir da posicão atual (bite 2048)*/
  /* copia á area do supebloco para &superbloco */
  fread(blocks_group_descriptor, 1, sizeof(Ext2_Blocks_Group_Descriptor), ext2_image);

  /*  */
  return blocks_group_descriptor;
}


void print_ext2_blocks_group_descriptor(Ext2_Blocks_Group_Descriptor* blocks_group_descriptor) {
  cout << "bg_block_bitmap:\t\t"          << blocks_group_descriptor->bg_block_bitmap              << endl;
  cout << "bg_inode_bitmap:\t\t"          << blocks_group_descriptor->bg_inode_bitmap              << endl;
  cout << "bg_inode_table:\t\t\t"         << blocks_group_descriptor->bg_inode_table               << endl;
  cout << "bg_free_blocks_count:\t\t"     << blocks_group_descriptor->bg_free_blocks_count         << endl;
  cout << "bg_free_inodes_count:\t\t"     << blocks_group_descriptor->bg_free_inodes_count         << endl;
  cout << "bg_used_dirs_count:\t\t"       << blocks_group_descriptor->bg_used_dirs_count           << endl;
  cout << "bg_pad:\t\t\t\t"               << blocks_group_descriptor->bg_pad                       << endl;
}

void write_ext2_blocks_group_descriptor(Ext2_Blocks_Group_Descriptor* bgd , FILE *ext2_image, uint32_t at_position) {
  fseek(ext2_image, at_position, SEEK_SET);
  fwrite(bgd, 1, sizeof(Ext2_Blocks_Group_Descriptor), ext2_image);
}

/*
* retorna a qual descritor de bloco o inode pertence.
* Obs: para o primeiro descritor de bloco, retorna 0 e assim por diante.
*/
unsigned int block_group_from_inode(Ext2_Superblock* superblock, unsigned int inode) {
  unsigned int inodes_per_block_group = superblock->s_inodes_per_group;
  return ( (inode - 1) / inodes_per_block_group );
}

unsigned int blocks_group_count(Ext2_Superblock* superblock) {
  return (unsigned) (superblock->s_blocks_count - 1) / superblock->s_blocks_per_group;
}

unsigned int size_of_blocks_group_descriptor_list(Ext2_Superblock* superblock){
  unsigned int group_count = blocks_group_count(superblock);

  return (unsigned) group_count * sizeof(Ext2_Blocks_Group_Descriptor);
}

uint32_t block_group_descriptor_address(int order) {
  return (unsigned int) 2048 + (sizeof(Ext2_Blocks_Group_Descriptor) * (order));
}