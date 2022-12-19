#include "../headers/ext2.hpp"
#include "../headers/util-operations.hpp"

/* protótipo da função que será implementada para procurar um bloco livre (comando: mkdir)*/
uint32_t find_free_block(unsigned int *store_bgd_index, Ext2_Superblock* superblock, FILE* ext2_img) {
  return 0;
}

/* retorna o byte no bitmap ao qual o bit de um determinado bloco pertence */
char get_byte_of_block_bitmap(Ext2_Blocks_Group_Descriptor* bgd, uint32_t byte_index, FILE* ext2_image){
  uint32_t absolut_bitmap_position = BLOCK_OFFSET(bgd->bg_block_bitmap);
  uint32_t absolut_byte_position = absolut_bitmap_position + byte_index;

  char byte = 0;

  fseek(ext2_image, absolut_byte_position, SEEK_SET);
  fread(&byte, 1, 1, ext2_image);
  return byte;
}

/* escreve um byte em sua posição adequada dentro do bitmap de bloco do descritor de bloco */
void set_byte_on_block_bitmap(char byte, Ext2_Blocks_Group_Descriptor* bgd, uint32_t byte_index, FILE* ext2_image) {
  uint32_t absolut_bitmap_position = BLOCK_OFFSET(bgd->bg_block_bitmap);
  uint32_t absolut_byte_position = absolut_bitmap_position + byte_index;

  fseek(ext2_image, absolut_byte_position, SEEK_SET);
  fwrite(&byte, 1, 1, ext2_image);
}

/* retorna o index do descritor de grupo ao qual o bloco pertence */
unsigned int block_group_from_block(Ext2_Superblock* superblock, unsigned int block) {
  unsigned int blocks_per_block_group = superblock->s_blocks_per_group;
  return ( (block - 1) / blocks_per_block_group );
}

/* retorna qual a posição do bloco dentro do grupo */
unsigned int bit_block_order_on_block_group(Ext2_Superblock *superblock, uint32_t block)
{
  return (unsigned int) block % superblock->s_blocks_per_group;
}