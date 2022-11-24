#include <iostream>

#include "../headers/file-operations.hpp"
#include "../headers/colors.hpp"

using namespace std;

FILE *get_file(const char *address)
{
  FILE *file = fopen(address, "rb+");
  if (file == NULL)
  {
    cout << string(RED) << "\n[ File " << address << " is not valid ]" << string(DEFAULT) << endl;
    throw("[ Error::getFile ]");
  }
  else
    cout << string(GREEN) << "\n[ File " << address << " read ]" << string(DEFAULT) << endl;

  return file;
}