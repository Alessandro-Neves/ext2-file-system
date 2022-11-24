#include "../headers/colors.hpp"
#include "../headers/ext2.hpp"
#include "../headers/file-operations.hpp"
#include "../headers/print-operations.hpp"

using namespace std;

int main()
{
  FILE *file;
  Ext2_Superblock *superblock;

  try
  {
    /* pegar endereço da image ext2 no host */
    char *input = (char *)malloc(sizeof(char) * 100);
    // cout << "[ Image address ]:\t";
    // cin >> input;
    strcpy(input, "./ext2.img"); /* !!!! apagar antes de entregar e descomentar as duas linhas anteriores !!!! */

    /* aloca memória para ler todo o superbloco */
    Ext2_Superblock *superblock = (Ext2_Superblock *)malloc(sizeof(Ext2_Superblock));

    /* ler imagem ext2 do host */
    FILE *file = get_file((const char *)input);

    /* posicionar o ponteiro no início do superbloco (bite 1024) */
    fseek(file, 1024, SEEK_SET);

    /* copiar 1024 unidade de 1byte a partir da posicão atual (bite 1024)*/
    /* copia á area do supebloco para &superbloco */
    fread(superblock, 1, 1024, file);

    /* imprime as informações do superbloco */
    print_superblock(superblock);
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