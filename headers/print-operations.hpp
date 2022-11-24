#include "./ext2.hpp"

#ifndef _PRINT_OPERATIONS_H_
#define _PRINT_OPERATIONS_H_

/**
 * @brief imprime os dados do superbloco na saída
 * @param superblock estrutura com dados do superbloco
 */
void print_superblock(Ext2_Superblock* superblock);
#endif