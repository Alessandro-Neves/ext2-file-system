#include <stdlib.h>

#include "./ext2.hpp"
#include "./colors.hpp"

#ifndef _BLOCK_GROUP_OPERATIONS_H_
#define _BLOCK_GROUP_OPERATIONS_H_

using namespace std;

/**
 * @brief realiza a leitura do descritor de grupo de blocos do sistema de arquivos e mapeia para Ext2_Blocks_Group_Descriptor
 * 
 * @param ext2_image imagem do sistema de arquivos
 * @return Ext2_Blocks_Group_Descriptor* estrutura com as informações do descritor de grupo de blocos
 */
Ext2_Blocks_Group_Descriptor *read_ext2_blocks_group_descriptor(FILE *ext2_image, uint32_t at_position);

/**
 * @brief imprime os dados do descritor de grupo de blocos na saída
 * @param superblock estrutura com dados do descritor de grupo de blocos
 */
void print_ext2_blocks_group_descriptor(Ext2_Blocks_Group_Descriptor* blocks_group_descriptor);

unsigned int blocks_group_count(Ext2_Superblock* superblock);

unsigned int size_of_blocks_group_descriptor_list(Ext2_Superblock* superblock);

uint32_t block_group_descriptor_address(int order);

#endif