#include "./ext2.hpp"
#include "./general-constants.hpp"

#ifndef _INODE_OPERATIONS_H_
#define _INODE_OPERATIONS_H_

Ext2_Inode *read_ext2_inode(FILE *ext2_image, Ext2_Blocks_Group_Descriptor *block_group_descriptor, int inode_order);

/**
 * @brief imprime os dados do inode na saída
 * @param inode estrutura com dados do inode
 */
void print_ext2_inode(Ext2_Inode* blocks_group_descriptor);

#endif