#include "../headers/util-operations.hpp"

bool has_null(const char *array, int size) {
  for (int i = 0; i < size; i++)
    if (!array[i])
      return true;
  return false;
}