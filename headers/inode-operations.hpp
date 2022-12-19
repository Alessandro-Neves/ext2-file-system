#include "./ext2.hpp"
#include "./general-constants.hpp"

#ifndef _INODE_OPERATIONS_H_
#define _INODE_OPERATIONS_H_

/* realiza a leitura de um determina inode referenciado na tabela de inodes do descritor de grupo */
Ext2_Inode *read_ext2_inode(FILE *ext2_image, Ext2_Blocks_Group_Descriptor *block_group_descriptor, unsigned int inode_order);

/* escreve o inode na imagem */
void write_ext2_inode(FILE *ext2_image, Ext2_Blocks_Group_Descriptor *block_group_descriptor, unsigned int inode_order);

/* realiza a impressão dos dados do 'inode' */
void print_ext2_inode(Ext2_Inode* inode);

/* realiza a impressão do conteudo dos blocos de dados do 'inode' */
void print_inode_blocks_content(FILE* ext2_image, Ext2_Inode* inode);

/* retorna a ordem (cardinalidade) do inode dentro do descritor de grupo */
unsigned int inode_order_on_block_group(Ext2_Superblock* superblock, uint32_t inode);

/* cria uma estrutura de inode padrão */
Ext2_Inode* create_default_inode();

bool set_bit_of_inode_bitmap(Ext2_Superblock* superblock , uint32_t inode, FILE* ext2_image);

/* retorna um determinado grupo de 8 bits (char) do bitmap de inodo do descritor de grupo referênciado  */
char get_byte_of_inode_bitmap(Ext2_Blocks_Group_Descriptor* bgd, uint32_t byte_index, FILE* ext2_image);

/* escreve o grupo de bits (char) no bitmap de inode do descritor de grupo referênciado */
void set_byte_on_inode_bitmap(char byte, Ext2_Blocks_Group_Descriptor* bgd, uint32_t byte_order, FILE* ext2_image);

/* encontra um inode livre para ser utilizado no sistema */
uint32_t find_free_inode(Ext2_Superblock* superblock, FILE* ext2_image);

/* realiza a cópia dos blocos de dados do 'inode' para o arquivo indicado por 'file_name' */
void copy_inode_blocks_content(FILE *ext2_image, Ext2_Inode *inode, const char* file_name, const char* path_destiny);

/* constroi uma string que informa permissões, tipo e dados básicos de um diretório/arquivo a partir do i_mode */
std::string permission_i_mode(uint32_t i_mode);
#endif