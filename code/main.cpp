#include "../headers/colors.hpp"
#include "../headers/ext2.hpp"
#include "../headers/file-operations.hpp"
#include "../headers/superblock-operations.hpp"
#include "../headers/blocks-group-operations.hpp"
#include "../headers/inode-operations.hpp"
#include "../headers/general-constants.hpp"
#include "../headers/directory-operations.hpp"

using namespace std;

int main()
{
  FILE *file;
  Ext2_Superblock *superblock;
  Ext2_Blocks_Group_Descriptor *blocks_group_descriptor;
  Ext2_Inode *inode;
  vector<Ext2_Directory> directories;

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
    blocks_group_descriptor = read_ext2_blocks_group_descriptor(ext2_image, 2048);

    /* leitura do primeiro inode (segundo inode do primeiro descritor de blocos) */
    inode = read_ext2_inode(ext2_image, blocks_group_descriptor, 2);

    /* leitura dos diretórios raiz */
    directories = read_ext2_directories(ext2_image, inode, blocks_group_descriptor);

    /* imprime as informações do superbloco */
    cout << string(BLUE) << endl << "[ Superblock infos ]" << string(DEFAULT) << endl;
    print_superblock(superblock);

    /* imprime as informações do descritor de grupo de blocos */
    cout << string(BLUE) << endl << "[ Blocks group descriptor infos ]" << string(DEFAULT) << endl;
    print_ext2_blocks_group_descriptor(blocks_group_descriptor);

    /* imprime as informações do inode */
    cout << string(BLUE) << endl << "[ Inode infos ]" << string(DEFAULT) << endl;
    print_ext2_inode(inode);

    /* imprime as informações dos diretórios */
    cout << string(BLUE) << endl << "[ Root Directories infos ]" << string(DEFAULT) << endl;
    print_directories(directories);

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