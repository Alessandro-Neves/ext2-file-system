#include "./ext2.hpp"

#ifndef _SUPERBLOCK_OPERATIONS_H_
#define _SUPERBLOCK_OPERATIONS_H_

/**
 * @brief realiza a leitura do superbloco do sistema de arquivos e mapeia para Ext2_Superblock
 * 
 * @param ext2_image imagem do sistema de arquivos
 * @return Ext2_Superblock* estrutura com as informações do superbloco
 */
Ext2_Superblock* read_ext2_superblock(FILE* ext2_image);

/**
 * @brief imprime os dados do superbloco na saída
 * @param superblock estrutura com dados do superbloco
 */
void print_superblock(Ext2_Superblock* superblock);

#endif