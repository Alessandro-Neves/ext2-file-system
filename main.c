#include "../headers/ext2.hpp"

Ext2_Blocks_Group_Descriptor *read_ext2_blocks_group_descriptor(FILE *ext2_image)
{

  /* aloca espaço na memória suficiente para armazenar a estrutura do Ext2_Blocks_Group_Descriptor */
  Ext2_Blocks_Group_Descriptor *blocks_group_descriptor = (Ext2_Blocks_Group_Descriptor *)malloc(sizeof(Ext2_Blocks_Group_Descriptor));

  /* posicionar o ponteiro no início do superbloco (bite 2048) */
  fseek(ext2_image, 2048, SEEK_SET);

  /* copiar 288 unidade de 1byte a partir da posicão atual (bite 2048)*/
  /* copia á area do supebloco para &superbloco */
  fread(blocks_group_descriptor, 1, 288, ext2_image);

  /*  */
  return blocks_group_descriptor;
}

#include<stdio.h>
#include<stdlib.h>

using namespace std;

int main()
{
  FILE *file;
  Ext2_Superblock *superblock;
  Ext2_Blocks_Group_Descriptor *blocks_group_descriptor;

  try
  {
    /* pegar endereço da image ext2 no host */
    char *input = (char *)malloc(sizeof(char) * 100);
    // cout << "[ Image address ]:\t";
    // cin >> input;
    strcpy(input, "./ext2.img"); /* !!!! apagar antes de entregar e descomentar as duas linhas anteriores !!!! */

    /* ler imagem ext2 do host */
    FILE *ext2_image = get_file((const char *)input);

    /* leitura da superbloco */
    superblock = read_ext2_superblock(ext2_image);

    /* leitura do descritor de grupo de blocos */
    // blocks_group_descriptor = read_ext2_blocks_group_descriptor(ext2_image);

    /* imprime as informações do superbloco */
    print_superblock(superblock);

    /* imprime as informações do descritor de grupo de blocos */
    print_ext2_blocks_group_descriptor(blocks_group_descriptor);

    // read_inodee(ext2_image, blocks_group_descriptor);

    int* i = (int*) malloc(sizeof(int));


  }
  catch (const char *str) /* tratamento de exceções */
  {
    cout << string(RED) << str << string(DEFAULT) << endl;
  }
  catch (...)
  {
    cout << string(RED) << "[ Unespected error ]" << string(DEFAULT) << endl;
  }

  return 0;
}