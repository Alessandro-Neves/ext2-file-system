#include <cstdbool>
#include "./general-constants.hpp"

#ifndef _UTIL_OPERATIONS_H_
#define _UTIL_OPERATIONS_H_

bool has_null(const char* array, int size);

unsigned int bytes_to_4_bytes_groups_length(unsigned int bytes);

#include <iostream>

using namespace std;

// Função para inverter a ordem dos bits de um char
char reverse_bits(char c);

/* 
  Função para setar um determinado bit do char para 0 ou para 1.
  O index 0 se refere ao bit menos significativo, o index 7 ao mais significativo,
  o restante dos indexes/bits segue a mesma lógica.
  exemplo: set_bit((char) 0b00000000, 1, true) retornará (char) 0b00000010
*/
char set_bit(char c, int index, bool value);

#endif