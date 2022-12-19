#include <sys/stat.h>
#include <ctime>
#include "../headers/util-operations.hpp"
#include "../headers/colors.hpp"

/* informa se algum elemento do 'array' é null ou zero ou '\0'  */
bool has_null(const char *array, int size)
{
  for (int i = 0; i < size; i++)
    if (!array[i])
      return true;
  return false;
}

/* informa o quantidade minima multiplo de 4 capas de armazenar determinada 
quantidade de bytes */
unsigned int bytes_to_4_bytes_groups_length(unsigned int bytes)
{
  const unsigned int base = bytes / 4;
  const unsigned int default_size = base * 4;
  return (bytes - (base * 4)) > 0 ? default_size + 4 : default_size;
}

// inverter a ordem dos bits de um char
char reverse_bits(char c)
{
  char inverted = 0;
  for (int i = 0; i < 8; i++)
  {
    inverted = (inverted << 1) | (c & 1);
    c = c >> 1;
  }
  return inverted;
}

// setar um determinado bit do char para 0 ou para 1
char set_bit(char c, int index, bool value)
{
  if (value)
    return c | (1 << index);
  else
    return c & ~(1 << index);
}

int find_first_zero_bit(char c)
{
  for (int i = 0; i < 8; i++)
  {
    if ((c & (1 << i)) == 0)
    {
      return i;
    }
  }
  return -1; // Nenhum bit 0 encontrado
}

/* informa se o diretório na caminho 'path' existe no sistema do usuário */
bool directory_exists(const char *path)
{
  struct stat st;
  if (stat(path, &st) == 0 && (st.st_mode & S_IFDIR))
    return true;
  else
    return false;
}


/* imprime o Unix timestemp 'tempo' no formato dia/mes/ano hora:minuto 
!!! retornando a terceira linha abaixo para evitar um bug encontrado que 
acontece em alguma máquinas Linux (o programa executando interrompe a execução) */
void print_time_from_unix(unsigned int tempo)
{
    std::cout << tempo << std::endl;  return;

    int timestamp = tempo;

    // Convert the timestamp to a time_t variable
    time_t time = timestamp;

    // Convert the time_t variable to a tm structure
    tm* timeinfo = localtime(&time);

    // Create a buffer to hold the formatted date and time string
    char buffer[20];

    // Use strftime to format the date and time
    strftime(buffer, 20, "%d/%m/%Y %H:%M", timeinfo);

    // Print the formatted string
    std::cout << buffer << std::endl;
}