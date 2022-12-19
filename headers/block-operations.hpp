#include <stdio.h>
#include "./ext2.hpp"

#ifndef _BLOCK_OPERATIONS_H_
#define _BLOCK_OPERATIONS_H_

/* procura por um bloco livre e então retorna sua posição absoluta */
uint32_t find_free_block(unsigned int *store_bgd_index, Ext2_Superblock* superblock, FILE* ext2_img);

/* retorna o index do descritor de grupo ao qual o bloco pertence */
unsigned int block_group_from_block(Ext2_Superblock* superblock, unsigned int block);

/* retorna qual a posição do bloco dentro do grupo */
unsigned int bit_block_order_on_block_group(Ext2_Superblock *superblock, uint32_t block);

/* escreve um byte em sua posição adequada dentro do bitmap de bloco do descritor de bloco */
void set_byte_on_block_bitmap(char byte, Ext2_Blocks_Group_Descriptor* bgd, uint32_t byte_index, FILE* ext2_image);

/* retorna o byte no bitmap ao qual o bit de um determinado bloco pertence */
char get_byte_of_block_bitmap(Ext2_Blocks_Group_Descriptor* bgd, uint32_t byte_index, FILE* ext2_image);

#endif