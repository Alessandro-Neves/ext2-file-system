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
  cout << "void:\t" << sizeof(void) << endl;
  return 0;
}

/*
  sizeof(unsigned char)    =   sizeof(uint8_t) = sizeof(void)
  sizeof(unsigned short)   =   sizeof(uint16_t)
  sizeof(unsigned int)     =   sizeof(uint32_t)
*/