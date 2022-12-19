#include "./ext2.hpp"

#ifndef _SUPERBLOCK_OPERATIONS_H_
#define _SUPERBLOCK_OPERATIONS_H_

/* realiza a leitura do superbloco do sistema de arquivos e mapeia para Ext2_Superblock */
Ext2_Superblock* read_ext2_superblock(FILE* ext2_image);

/* escreve a estrutura de superbloco na imagem do sistema de arquivos */
void write_ext2_superblock(Ext2_Superblock* superblock, FILE* ext2_image);

/* realiza a impressão dos dados do superbloco */
void print_superblock(Ext2_Superblock* superblock);

#endif