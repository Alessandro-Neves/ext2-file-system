#include "../headers/colors.hpp"
#include "../headers/ext2.hpp"
#include "../headers/file-operations.hpp"
#include "../headers/superblock-operations.hpp"
#include "../headers/blocks-group-operations.hpp"
#include "../headers/inode-operations.hpp"
#include "../headers/general-constants.hpp"
#include "../headers/directory-operations.hpp"
#include "../headers/ext2-file-manager.hpp"

void shell (Ext2FileManager* fm) {
  system("clear");
  std::string input, operation, argument;

  while(1){
    fflush(stdout);
    std::cout << std::endl << "[ " << fm->pwd() << " ]>  ";
    std::getline(std::cin, input);

    operation = input.substr(0, input.find(" "));

    int pos = input.find(" ") + 1;
    argument = input.substr(pos, argument.length() - pos);

    if(!std::strcmp(operation.c_str(), "cd"))  
      fm->cd(argument.c_str());
    
    else if(!std::strcmp(operation.c_str(), "ls"))
      fm->ls();

    else if(!std::strcmp(operation.c_str(), "cat"))
      fm->cat(argument.c_str());

    else std::cout << "comand not found !" << std::endl;
  }
}

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

    // cout << endl << string(BLUE) << "superblock info:" << string(DEFAULT) << endl;
    // fm->info_superblock();  

    // cout << endl << string(BLUE) << "blocks group descriptor info:" << string(DEFAULT) << endl;
    // fm->info_blocks_group_descriptor();

    // cout << endl << string(BLUE) << "root inode info:" << string(DEFAULT) << endl;
    // fm->info_inode();

    // cout << endl << string(BLUE) << "ls /:" << string(DEFAULT) << endl;
    // fm->ls();

    // cout << endl << string(BLUE) << "lendo conteudo de ./hello.txt :" << string(DEFAULT)<< endl;
    // fm->cat("hello.txt");

    // cout << endl << string(BLUE) << "navegando para ./livros :" << string(DEFAULT)<< endl;
    // fm->cd("livros");

    // cout << endl << string(BLUE) << "inode ./livros :" << string(DEFAULT)<< endl;
    // fm->info_inode();

    // cout << endl << string(BLUE) << "ls ./livros :" << string(DEFAULT)<< endl;
    // fm->ls();

    // cout << endl << string(BLUE) << "cd ./livros/religiosos:" << string(DEFAULT)<< endl;
    // fm->cd("religiosos");

    // cout << endl << string(BLUE) << "ls ./livros/religiosos:" << string(DEFAULT)<< endl;
    // fm->ls();

    // cout << endl << string(BLUE) << "cat ./livros/religiosos/Biblia.txt:" << string(DEFAULT)<< endl;
    // fm->cat("Biblia.txt");

    shell(fm);

  }
  catch (const char *str) /* tratamento de exceções */
  {
    cout << string(RED) << str << string(DEFAULT) << endl;
  }
  // catch (...)
  // {
  //   cout << string(RED) << "[ Unespected error ]" << string(DEFAULT) << endl;
  // }

  return 0;
}