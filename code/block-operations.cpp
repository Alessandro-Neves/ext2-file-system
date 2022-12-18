#include "../headers/ext2.hpp"
#include "../headers/util-operations.hpp"

uint32_t find_free_block(unsigned int *store_bgd_index, Ext2_Superblock* superblock, FILE* ext2_img) {
  return 0;
}

char get_byte_of_block_bitmap(Ext2_Blocks_Group_Descriptor* bgd, uint32_t byte_index, FILE* ext2_image){
  uint32_t absolut_bitmap_position = BLOCK_OFFSET(bgd->bg_block_bitmap);
  uint32_t absolut_byte_position = absolut_bitmap_position + byte_index;

  char byte = 0;

  fseek(ext2_image, absolut_byte_position, SEEK_SET);
  fread(&byte, 1, 1, ext2_image);
  return byte;
}

void set_byte_on_block_bitmap(char byte, Ext2_Blocks_Group_Descriptor* bgd, uint32_t byte_index, FILE* ext2_image) {
  uint32_t absolut_bitmap_position = BLOCK_OFFSET(bgd->bg_block_bitmap);
  uint32_t absolut_byte_position = absolut_bitmap_position + byte_index;

  fseek(ext2_image, absolut_byte_position, SEEK_SET);
  fwrite(&byte, 1, 1, ext2_image);
}

/*
* retorna a qual descritor de bloco o bloco pertence.
* Obs: para o primeiro descritor de bloco, retorna 0 e assim por diante.
*/
unsigned int block_group_from_block(Ext2_Superblock* superblock, unsigned int block) {
  unsigned int blocks_per_block_group = superblock->s_blocks_per_group;
  return ( (block - 1) / blocks_per_block_group );
}

/**
 * retorna a ordem do bit de bloco dentro do descritor de grupo
 * Obs: Para o primeiro bit do descritor retorna 1
 */
unsigned int bit_block_order_on_block_group(Ext2_Superblock *superblock, uint32_t block)
{
  return (unsigned int) block % superblock->s_blocks_per_group;
}