#ifndef LIB_AUX_TOOLS
#define LIB_AUX_TOOLS

#include <cstring>
#include <iostream>
#include <stdlib.h>

#include "db_types.h"

#define MAX_VAL 1000000
#define MAX_DBL 100.0

// Shows string 'str' in 'stdout'
void message(word str);

// Converts "const char *" to "std::string"
word ccp_to_str(const char *str);

// Returns a random number in [0.0, 1.0] interval
double rand_prob(unsigned int *seed);

// Returns a random string with letters in [a-zA-Z] interval
word rand_string(int size, unsigned int *seed);

// Returns a random double number
double rand_value(unsigned int *seed);

#endif