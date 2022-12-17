#include <iostream>
#include <bitset>

#include "../headers/util-operations.hpp"

int main() {
  char a = (char) 0b11010001;
  char reversed = reverse_bits(a);
  std::cout << "Bits '" << std::bitset<8>(a) << "' invertido para '" << std::bitset<8>(reversed) << std::endl;

  char b = (char) 0b00000110; // char com os bits 11011001
  char modified = set_bit(b, 7, true); // deve retornar 11111001
  std::cout << "Bits '" << std::bitset<8>(b) << "' modificado para '" << std::bitset<8>(modified) << std::endl;
  return 0;
}