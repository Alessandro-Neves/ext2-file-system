#include "../headers/ext2.hpp"
#include "../headers/util-operations.hpp"

uint32_t find_free_block(unsigned int *store_bgd_index, Ext2_Superblock* superblock, FILE* ext2_img) {
  return 0;
}

char get_byte_of_inode_bitmap(Ext2_Blocks_Group_Descriptor* bgd, uint32_t byte_order, FILE* ext2_image){
  //
}

void set_byte_on_block_bitmap(char byte, Ext2_Blocks_Group_Descriptor* bgd, uint32_t byte_order, FILE* ext2_image) {
  //
}