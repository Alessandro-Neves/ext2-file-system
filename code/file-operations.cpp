#include <iostream>

#include "../headers/file-operations.hpp"
#include "../headers/colors.hpp"
#include "../headers/error.hpp"

using namespace std;

/* realiza a leitura de um arquivo na máquina do usuário */
FILE *get_file(const char *address)
{
  FILE *file = fopen(address, "rb+");
  if (file == NULL) cout << string(RED) << "\n[ File " << address << " is not valid ]" << string(DEFAULT) << endl << endl;
  else  cout << string(GREEN) << "\n[ File " << address << " read ]" << string(DEFAULT) << endl;
  return file;
}

/* realiza a movimentação 'shift' de uma determinado porção de bytes na imagem 
(!! será utilizado futuramente) */
void shift_bytes(FILE *ext2_image, uint32_t position, unsigned int qtd_bytes, unsigned int offset)
{
  try
  {
    char *buffer = (char *)malloc(sizeof(char) * qtd_bytes);

    fseek(ext2_image, position, SEEK_SET);
    fread(buffer, 1, qtd_bytes, ext2_image);

    fseek(ext2_image, position + offset, SEEK_SET);
    fwrite(buffer, 1, qtd_bytes, ext2_image);
  }
  catch (...)
  {
    throw new Error("shift_bytes");
  }

}