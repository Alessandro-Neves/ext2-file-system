#include <cstdbool>
#include "./general-constants.hpp"

#ifndef _UTIL_OPERATIONS_H_
#define _UTIL_OPERATIONS_H_

bool has_null(const char* array, int size);

unsigned int bytes_to_4_bytes_groups_length(unsigned int bytes);

#endif