#include <stdlib.h>
#include "./ext2.hpp"
#include "./colors.hpp"

#ifndef _BLOCK_GROUP_OPERATIONS_H_
#define _BLOCK_GROUP_OPERATIONS_H_

using namespace std;

/* realiza a leitura e retorna um descritor de grupo dentro da imagem  */
Ext2_Blocks_Group_Descriptor *read_ext2_blocks_group_descriptor(FILE *ext2_image, uint32_t at_position);

/* realiza a impressão de dados do descritor de grupo */
void print_ext2_blocks_group_descriptor(Ext2_Blocks_Group_Descriptor* blocks_group_descriptor);

/* retorna o endereço absoluto do descritor de grupo na imagem */
unsigned int block_group_descriptor_from_inode(Ext2_Superblock* superblock, unsigned int inode_order);

uint32_t block_group_descriptor_address(int bgd_index);

/* retorna o index do descritor de grupo ao qual o 'inode' pertence */
unsigned int block_group_from_inode(Ext2_Superblock* superblock, unsigned int inode);

/* escreve o descritor de grupo 'bgd' na imagem 'ext2_image' na posição 'at_position' */
void write_ext2_blocks_group_descriptor(Ext2_Blocks_Group_Descriptor* bgd , FILE *ext2_image, uint32_t at_position);

#endif