#include <stdio.h>
#include "./ext2.hpp"

#ifndef _BLOCK_OPERATIONS_H_
#define _BLOCK_OPERATIONS_H_

uint32_t find_free_block(unsigned int *store_bgd_index, Ext2_Superblock* superblock, FILE* ext2_img);

unsigned int block_group_from_block(Ext2_Superblock* superblock, unsigned int block);

unsigned int bit_block_order_on_block_group(Ext2_Superblock *superblock, uint32_t block);

void set_byte_on_block_bitmap(char byte, Ext2_Blocks_Group_Descriptor* bgd, uint32_t byte_index, FILE* ext2_image);

char get_byte_of_block_bitmap(Ext2_Blocks_Group_Descriptor* bgd, uint32_t byte_index, FILE* ext2_image);

#endif