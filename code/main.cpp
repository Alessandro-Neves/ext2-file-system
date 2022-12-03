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
  std::string input, operation, argument;

  while(1){
    fflush(stdout);
    std::cout << std::endl << "[ " << fm->pwd() << " ]>  " << string(BLUE);
    std::getline(std::cin, input);
    std::cout << std::string(DEFAULT) << std::endl;

    operation = input.substr(0, input.find(" "));

    int pos = input.find(" ") + 1;
    argument = input.substr(pos, input.length() - pos);

    if(!std::strcmp(operation.c_str(), "cd"))  
      fm->cd(argument.c_str());
    
    else if(!std::strcmp(operation.c_str(), "ls"))
      fm->ls();

    else if(!std::strcmp(operation.c_str(), "cat"))
      fm->cat(argument.c_str());

    else if(!std::strcmp(operation.c_str(), "print-inode")){
      int inode = std::stoi(argument);
      fm->info_inode(inode);
    }

    else if(!std::strcmp(operation.c_str(), "clear"))
      system("clear");

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

    // fm->cd("livros");
    // fm->cd("religiosos");
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