#include <cstdbool>
#include "./ext2.hpp"
#include "./general-constants.hpp"

using namespace std;

#ifndef _EXT2_FILE_MANAGER_H_
#define _EXT2_FILE_MANAGER_H_

class Ext2FileManager
{
private:
  FILE *ext2_image = NULL;
  Ext2_Superblock *superblock = NULL;
  Ext2_Blocks_Group_Descriptor *blocks_group_descriptor = NULL;
  vector<Ext2_Directory> history_navigation;

  /* realiza a liberação 'free' dos blocos utilizados por um determinado 'inode' */
  void release_blocks_of_inode(Ext2_Inode *inode);

  /* realiza a liberação 'free' dos blocos do array 'blocks' */
  void release_blocks(uint32_t *blocks, int blocks_size, uint32_t *bytes_to_remove);

  /* realiza a leitura de um determinado 'inode' na imagem */
  Ext2_Inode *_read_ext2_inode(uint32_t inode);

public:

  /* construtor do gerenciador/manipulador do sistema de arquivos, 
  realiza a inicialização e leitura de informações importantes como o arquivo da imagem 
  e a leitura do diretório principal */
  Ext2FileManager(FILE *ext2_image);

  /* realiza a impressão dos dados do superbloco */
  void info_superblock();

  /* realiza a impressão dos dados de um determinado descritor de grupo identificado pelo index
  no bloco de descritores */
  void info_blocks_group_descriptor(int bgd_index);

  /* realiza a impressão de dados do 'inode' */
  void info_inode(unsigned int inode);

  /* realiza a impressão de dados importantes do sistema de arquivos */
  void info();

  /* realiza a impressão de das permissões, tipo e dados básicos de um diretório/arquivo */
  void attr(const char *directory_name);

  /* realiza a navegação para um determinado diretório, verificando antes a existência do mesmo e a validade da operação */
  void cd(const char *directory_name);

  /* realiza a listagem dos diretórios contidos no diretório referenciado atualmente pela navegação */
  void ls();

  /* realiza a criação e inicialização de um novo diretório/arquivos no sistema */
  void touch(const char *directory_name, unsigned int directory_name_length);

  /* realiza a renomeação de um determina diretório/arquivo contido dentro do diretório referênciado atualmente pela navegação */
  void rename(const char *directory_name, const char *new_directory_name, unsigned int new_directory_name_length);

  /* realiza a remoção de um determinado arquivo, executando antes as verificações necessárias */
  void rm(const char *directory_name, unsigned int directory_name_length, bool info);

  /* realiza a cópia de um determinado arquivo interno para um determinado diretório do sistema do usuário */
  void copy(const char *origin_name, const char *destiny_name, bool info);

  /* realiza a impressão do conteudo contido em uma determinado arquivo */
  void cat(const char *directory_name);

  void move(const char *origin_name, const char *destiny_name);

  /* seta o bit de um determinado 'inode' no bitmap, realizado as verificações, 
  e atualizado os valores do superbloco e do descritor de grupo (inodes livres) */
  void set_bit_of_inode_bitmap(unsigned int inode, bool value);

  /* seta o bit de um determinado bloco no bitmap, realizado as verificações, 
  e atualizado os valores do superbloco e do descritor de grupo (blocos livres) */
  void set_bit_of_block_bitmap(unsigned int block, bool value);

  /* imprime o grupo de 8 bits dentro do inode bitmap ao qual o bit referente ao 'inode' pertence */
  void print_byte_on_bitmap_of_inode(unsigned int inode);

  /* imprime o grupo de 8 bits dentro do block bitmap ao qual o bit referente ao 'block' pertence */
  void print_byte_on_bitmap_of_block(unsigned int block);

  /* constroi uma string que informa o caminha 'path' do diretório atual referênciado pela navegação */
  std::string pwd();
};

#endif