#include <sys/stat.h>
#include <iostream>

int main() {
  // Verifica se o diretório "meu_diretorio" existe
  struct stat st;
  if (stat("/home/alessandro/ext2", &st) == 0 && (st.st_mode & S_IFDIR)) {
    std::cout << "O diretório existe." << std::endl;
  } else {
    std::cout << "O diretório não existe." << std::endl;
  }

  return 0;
}