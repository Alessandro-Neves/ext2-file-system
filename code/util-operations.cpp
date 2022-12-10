#include "../headers/util-operations.hpp"

bool has_null(const char *array, int size) {
  for (int i = 0; i < size; i++)
    if (!array[i])
      return true;
  return false;
}

unsigned int bytes_to_4_bytes_groups_length(unsigned int bytes) {
  const unsigned int base = bytes / 4;
  const unsigned int default_size = base * 4;
  return (bytes - (base * 4)) > 0 ? default_size + 4 : default_size;
}

void print_time_from_unix(unsigned int tempo)
{
    time_t epch = tempo;
    char time_buf[80];
    strftime(time_buf, sizeof(time_buf), "%a %Y-%m-%d %H:%M:%S %Z", gmtime(&epch));
    std::cout << time_buf << std::endl;
    
}