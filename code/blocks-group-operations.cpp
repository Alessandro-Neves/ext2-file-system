#include "../headers/blocks-group-operations.hpp"

/* realiza a leitura e retorna um descritor de grupo dentro da imagem  */
Ext2_Blocks_Group_Descriptor *read_ext2_blocks_group_descriptor(FILE *ext2_image, uint32_t at_position)
{

  /* aloca espaço na memória suficiente para armazenar a estrutura do Ext2_Blocks_Group_Descriptor */
  Ext2_Blocks_Group_Descriptor *blocks_group_descriptor = (Ext2_Blocks_Group_Descriptor *)malloc(sizeof(Ext2_Blocks_Group_Descriptor));

  /* posicionar o ponteiro no início do superbloco (bite 2048) e copia a area do supebloco para &superbloco*/
  fseek(ext2_image, at_position, SEEK_SET);
  fread(blocks_group_descriptor, 1, sizeof(Ext2_Blocks_Group_Descriptor), ext2_image);

  return blocks_group_descriptor;
}

/* realiza a impressão de dados do descritor de grupo */
void print_ext2_blocks_group_descriptor(Ext2_Blocks_Group_Descriptor* blocks_group_descriptor) {
  cout << "block bitmap:\t\t"          << blocks_group_descriptor->bg_block_bitmap              << endl;
  cout << "inode bitmap:\t\t"          << blocks_group_descriptor->bg_inode_bitmap              << endl;
  cout << "inode table:\t\t"         << blocks_group_descriptor->bg_inode_table               << endl;
  cout << "free blocks count:\t"     << blocks_group_descriptor->bg_free_blocks_count         << endl;
  cout << "free inodes count:\t"     << blocks_group_descriptor->bg_free_inodes_count         << endl;
  cout << "used dirs count:\t"       << blocks_group_descriptor->bg_used_dirs_count           << endl;
}

/* escreve o descritor de grupo 'bgd' na imagem 'ext2_image' na posição 'at_position' */
void write_ext2_blocks_group_descriptor(Ext2_Blocks_Group_Descriptor* bgd , FILE *ext2_image, uint32_t at_position) {
  fseek(ext2_image, at_position, SEEK_SET);
  fwrite(bgd, 1, sizeof(Ext2_Blocks_Group_Descriptor), ext2_image);
}

/* retorna o index do descritor de grupo ao qual o 'inode' pertence */
unsigned int block_group_from_inode(Ext2_Superblock* superblock, unsigned int inode) {
  unsigned int inodes_per_block_group = superblock->s_inodes_per_group;
  return ( (inode - 1) / inodes_per_block_group );
}

/* retorna o endereço absoluto do descritor de grupo na imagem */
uint32_t block_group_descriptor_address(int bgd_index) {
  return (unsigned int) 2048 + (sizeof(Ext2_Blocks_Group_Descriptor) * (bgd_index));
}