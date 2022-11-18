#include<iostream>
#include<cstdint>

using namespace std;
int main(){
  cout << "char:\t" << sizeof(unsigned char) << endl;
  cout << "short:\t" << sizeof(unsigned short) << endl;
  cout << "int:\t" << sizeof(unsigned int) << endl;
  cout << "int8:\t" << sizeof(int8_t) << endl;
  cout << "uint8:\t" << sizeof(uint8_t) << endl;
  cout << "uint16:\t" << sizeof(uint16_t) << endl;
  cout << "uint32:\t" << sizeof(uint32_t) << endl;
  return 0;
}

/*
  unsigned char    =   uint8_t
  unsigned short   =   uint16_t
  unsigned int     =   uint32_t
*/