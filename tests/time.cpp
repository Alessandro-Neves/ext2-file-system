#include <ctime>
#include <iostream>
#include <string>

int main() {
  int timestamp = 1623372400;
  time_t time = timestamp;
  tm* timeinfo = localtime(&time);
  char buffer[20];
  strftime(buffer, 20, "%d/%m/%Y %H:%M", timeinfo);
  std::cout << buffer << std::endl;

  return 0;
}
