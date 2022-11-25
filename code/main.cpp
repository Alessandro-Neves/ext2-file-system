#include "../headers/colors.hpp"
#include "../headers/ext2.hpp"
#include "../headers/file-operations.hpp"
#include "../headers/blocks-group-operations.hpp"
#include "../headers/superblock-operations.hpp"

#include <iostream>

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
    blocks_group_descriptor = read_ext2_blocks_group_descriptor(ext2_image);

    /* imprime as informações do superbloco */
    print_superblock(superblock);

    /* imprime as informações do descritor de grupo de blocos */
    print_ext2_blocks_group_descriptor(blocks_group_descriptor);
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