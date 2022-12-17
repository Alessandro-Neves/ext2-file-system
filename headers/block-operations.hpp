#include <stdio.h>
#include "./ext2.hpp"

#ifndef _BLOCK_OPERATIONS_H_
#define _BLOCK_OPERATIONS_H_

uint32_t find_free_block(unsigned int *store_bgd_index, Ext2_Superblock* superblock, FILE* ext2_img);

#endif