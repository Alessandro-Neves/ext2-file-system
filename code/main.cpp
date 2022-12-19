#include "../headers/colors.hpp"
#include "../headers/ext2.hpp"
#include "../headers/file-operations.hpp"
#include "../headers/superblock-operations.hpp"
#include "../headers/blocks-group-operations.hpp"
#include "../headers/inode-operations.hpp"
#include "../headers/general-constants.hpp"
#include "../headers/directory-operations.hpp"
#include "../headers/ext2-file-manager.hpp"
#include "../headers/error.hpp"

#include "../headers/util-operations.hpp"

void shell(Ext2FileManager *fm);

int main()
{
  FILE *ext2_image;
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

    shell(fm);
  }
  catch (const char *str) /* tratamento de exceções */
  {
    cout << string(RED) << str << string(DEFAULT) << endl;
  }
  catch (Error *error)
  {
    cout << endl
         << string(RED) << "[ error::" << error->message << " ]" << string(DEFAULT) << endl;
  }

  return 0;
}



/* shell de iteração com o usuário */
void shell(Ext2FileManager *fm)
{
  std::string input, operation, argument;

  while (1)
  {
    try
    {
      fflush(stdout);
      std::cout << std::endl
                << "[ " << fm->pwd() << " ]>  " << string(BLUE);
      std::getline(std::cin, input);
      std::cout << std::string(DEFAULT) << std::endl;

      operation = input.substr(0, input.find(" "));

      int pos = input.find(" ") + 1;
      argument = input.substr(pos, input.length() - pos);

      if (!std::strcmp(operation.c_str(), "cd"))
        fm->cd(argument.c_str());

      else if (!std::strcmp(operation.c_str(), "ls"))
      {
        fm->ls();
      }
      else if (!std::strcmp(operation.c_str(), "pwd")){
        string pwd(fm->pwd());

        if(pwd.size() > 1)  pwd.pop_back();
        cout << pwd << endl;
      }
      else if (!std::strcmp(operation.c_str(), "cat"))
        fm->cat(argument.c_str());

      else if (!std::strcmp(operation.c_str(), "rm"))
        fm->rm(argument.c_str(), argument.size(), true);

      else if (!std::strcmp(operation.c_str(), "info"))
        fm->info();

      else if (!std::strcmp(operation.c_str(), "print"))
      {
        std::string first_argument = argument.substr(0, argument.find(" "));
        int prox = argument.find(" ") + 1;
        std::string second_argument = argument.substr(prox, argument.length() - prox);

        if (!std::strcmp(first_argument.c_str(), "inode"))
        {
          int inode = std::stoi(second_argument);
          fm->info_inode(inode);
        }

        else if (!std::strcmp(first_argument.c_str(), "superblock"))
        {
          fm->info_superblock();
        }

        else if (!std::strcmp(first_argument.c_str(), "groups"))
        {
          for (int i = 0; i < 8; i++)
          {
            cout << string(BOLD) << "Block Group Descriptor " << i << ":" << string(DEFAULT) << endl;
            fm->info_blocks_group_descriptor(i);
            cout << endl;
          }
        }

        else if (!std::strcmp(first_argument.c_str(), "group"))
        {
          int bgd_index = std::stoi(second_argument);
          cout << string(BOLD) << "Block Group Descriptor " << bgd_index << ":" << string(DEFAULT) << endl;
          fm->info_blocks_group_descriptor(bgd_index);
        }

        else
          throw new FileManagerInfo("command not found");
      }

      else if (!std::strcmp(operation.c_str(), "bitmapi"))
      {
        int inode = std::stoi(argument);
        fm->print_byte_on_bitmap_of_inode(inode);
      }

      else if (!std::strcmp(operation.c_str(), "bitmapb"))
      {
        int block = std::stoi(argument);
        fm->print_byte_on_bitmap_of_block(block);
      }

      else if (!std::strcmp(operation.c_str(), "seti"))
      {
        std::string inode = argument.substr(0, argument.find(" "));

        int prox = argument.find(" ") + 1;
        std::string value = argument.substr(prox, argument.length() - prox);

        int int_inode = std::stoi(inode);
        bool bool_value = (bool)(std::stoi(value));
        fm->set_bit_of_inode_bitmap(int_inode, bool_value);
      }

      else if (!std::strcmp(operation.c_str(), "setb"))
      {
        std::string block = argument.substr(0, argument.find(" "));

        int prox = argument.find(" ") + 1;
        std::string value = argument.substr(prox, argument.length() - prox);

        int int_block = std::stoi(block);
        bool bool_value = (bool)(std::stoi(value));
        fm->set_bit_of_block_bitmap(int_block, bool_value);
      }

      else if (!std::strcmp(operation.c_str(), "touch"))
      {
        fm->touch(argument.c_str(), argument.size());
      }

      else if (!std::strcmp(operation.c_str(), "clear"))
        system("clear");

      else if (!std::strcmp(operation.c_str(), "rename"))
      {

        std::string directory_name = argument.substr(0, argument.find(" "));

        int prox = argument.find(" ") + 1;
        std::string new_directory_name = argument.substr(prox, argument.length() - prox);

        fm->rename(directory_name.c_str(), new_directory_name.c_str(), new_directory_name.size());
      }

      else if (!std::strcmp(operation.c_str(), "attr"))
      {
        fm->attr(argument.c_str());
      }

      else if (!std::strcmp(operation.c_str(), "cp"))
      {

        std::string directory_name = argument.substr(0, argument.find(" "));

        int prox = argument.find(" ") + 1;
        std::string new_directory_name = argument.substr(prox, argument.length() - prox);

        fm->copy(directory_name.c_str(), new_directory_name.c_str(), true);
      }

      else if (!std::strcmp(operation.c_str(), "mv"))
      {

        std::string directory_name = argument.substr(0, argument.find(" "));

        int prox = argument.find(" ") + 1;
        std::string new_directory_name = argument.substr(prox, argument.length() - prox);

        fm->move(directory_name.c_str(), new_directory_name.c_str());
      }

      else
        throw new FileManagerInfo("command not found");
    }
    catch (FileManagerInfo *file_manager_info)
    {
      std::cout << file_manager_info->message << endl;
    }
  }
}