#include <cstdbool>
#include <iostream>

#include "./general-constants.hpp"

using namespace std;

#ifndef _UTIL_OPERATIONS_H_
#define _UTIL_OPERATIONS_H_

/* informa se algum elemento do 'array' é null ou zero ou '\0'  */
bool has_null(const char* array, int size);

/* informa o quantidade minima multiplo de 4 capas de armazenar determinada 
quantidade de bytes */
unsigned int bytes_to_4_bytes_groups_length(unsigned int bytes);

// inverter a ordem dos bits de um char
char reverse_bits(char c);

/* 
  setar um determinado bit do char para 0 ou para 1.
  O index 0 se refere ao bit menos significativo, o index 7 ao mais significativo,
  o restante dos indexes/bits segue a mesma lógica.
  exemplo: set_bit((char) 0b00000000, 1, true) retornará (char) 0b00000010
*/
char set_bit(char c, int index, bool value);

/*
  procurar o index do primeiro bit 0 (zero) no byte,  
  a cardinalidade do index é em relação ao bit menos significativo.
  Por exemplo: find_first_zero_bit((char) 0b11101011) retornará 2, 
  onde 2 se refere ao 3º bit menos significativo do byte.
  Retorna -1 quando não existe um bit zero no byte.
*/ 
int find_first_zero_bit(char c);

/* informa se o diretório na caminho 'path' existe no sistema do usuário */
bool directory_exists(const char* path);

/* imprime o Unix timestemp 'tempo' no formato dia/mes/ano hora:minuto */
void print_time_from_unix(unsigned int tempo);

#endif