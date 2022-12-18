#include "./ext2.hpp"
#include "./general-constants.hpp"

#ifndef _INODE_OPERATIONS_H_
#define _INODE_OPERATIONS_H_

Ext2_Inode *read_ext2_inode(FILE *ext2_image, Ext2_Blocks_Group_Descriptor *block_group_descriptor, unsigned int inode_order);

void write_ext2_inode(FILE *ext2_image, Ext2_Blocks_Group_Descriptor *block_group_descriptor, unsigned int inode_order);

/**
 * brief: imprime os dados do inode na saída.
 * param: inode estrutura com dados do inode.
 */
void print_ext2_inode(Ext2_Inode* inode);

void print_inode_blocks_content(FILE* ext2_image, Ext2_Inode* inode);

unsigned int inode_order_on_block_group(Ext2_Superblock* superblock, uint32_t inode);

/**
 * brief: cria um inode com propriedades setadas com valores padrões.
 * return: Ext2_Inode* ponteiro para o inode criado.
 */
Ext2_Inode* create_default_inode();

bool set_bit_of_inode_bitmap(Ext2_Superblock* superblock , uint32_t inode, FILE* ext2_image);

char get_byte_of_inode_bitmap(Ext2_Blocks_Group_Descriptor* bgd, uint32_t byte_index, FILE* ext2_image);

void set_byte_on_inode_bitmap(char byte, Ext2_Blocks_Group_Descriptor* bgd, uint32_t byte_order, FILE* ext2_image);

uint32_t find_free_inode(Ext2_Superblock* superblock, FILE* ext2_image);

void copy_inode_blocks_content(FILE *ext2_image, Ext2_Inode *inode, const char* path);

void copy_inode_blocks_content(FILE *ext2_image, Ext2_Inode *inode, const char* file_name, const char* path_destiny);

std::string permission_i_mode(uint32_t i_mode);
#endif