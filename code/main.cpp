#include "../headers/colors.hpp"
#include "../headers/ext2.hpp"
#include "../headers/file-operations.hpp"
#include "../headers/superblock-operations.hpp"
#include "../headers/blocks-group-operations.hpp"
#include "../headers/inode-operations.hpp"
#include "../headers/general-constants.hpp"
#include "../headers/directory-operations.hpp"
#include "../headers/ext2-file-manager.hpp"

using namespace std;

int main()
{
  FILE* ext2_image;
  Ext2FileManager *fm;

  try
  {
    /* pegar endereço da image ext2 no host */
    char *input = (char *)malloc(sizeof(char) * 100);
    // cout << "[ Image address ]:\t";
    // cin >> input;
    strcpy(input, "./ext2.img"); /* !!!! apagar antes de entregar e descomentar as duas linhas anteriores !!!! */

    /* ler imagem ext2 do host */
    ext2_image = get_file((const char *)input);

    fm = new Ext2FileManager(ext2_image);

    cout << endl << string(BLUE) << "superblock info:" << string(DEFAULT) << endl;
    fm->info_superblock();  

    cout << endl << string(BLUE) << "blocks group descriptor info:" << string(DEFAULT) << endl;
    fm->info_blocks_group_descriptor();

    cout << endl << string(BLUE) << "root inode info:" << string(DEFAULT) << endl;
    fm->info_inode();

    cout << endl << string(BLUE) << "ls /:" << string(DEFAULT) << endl;
    fm->ls();

    cout << endl << string(BLUE) << "lendo conteudo de ./hello.txt :" << string(DEFAULT)<< endl;
    fm->cat("hello.txt");

    cout << endl << string(BLUE) << "navegando para ./lost+found :" << string(DEFAULT)<< endl;
    fm->cd("lost+found");

    cout << endl << string(BLUE) << "inode ./lost+found :" << string(DEFAULT)<< endl;
    fm->info_inode();


    cout << endl << string(BLUE) << "ls ./lost+found :" << string(DEFAULT)<< endl;
    fm->ls();

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