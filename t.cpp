#include <ctime>
#include <iostream>
#include <string>

int main() {
  // Unix timestamp (number of seconds since Jan 1, 1970)
  int timestamp = 1623372400;

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

  return 0;
}
