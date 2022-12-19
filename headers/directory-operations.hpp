#include "./ext2.hpp"

#ifndef _DIRECTORY_OPERATIONS_H_
#define _DIRECTORY_OPERATIONS_H_

using namespace std;

/* realiza a leitura dos diretórios contidos dentro do diretório identificado pelo 'inode' */
vector<Ext2_Directory> read_ext2_directories(FILE* ext2_image, Ext2_Inode* inode);

/* procura pelo diretório com o nome 'searched_name' entre os diretório contidos dentro do diretório identificado pelo 'inode' */
Ext2_Directory* search_directory(FILE* ext2_image, Ext2_Inode* inode, const char* searched_name);

/* procura pelo diretório com o nome 'searched_name' entre os diretório contidos dentro do diretório identificado pelo 'inode', 
informando também a posição do diretório, se encontrado, dentro da imagem.  */
Ext2_Directory* search_directory_and_position(FILE* ext2_image, Ext2_Inode* inode, const char* searched_name, unsigned int* store_position_of_directory_on_block);

/* realiza a impressão de informações uteis da estrutura de diretório */
void print_directory(Ext2_Directory directory);

/* realiza a impressão de informações uteis das estruturas de diretório */
void print_directories(vector<Ext2_Directory> directories);

#endif