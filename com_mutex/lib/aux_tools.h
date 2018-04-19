#ifndef LIB_AUX_TOOLS_H
#define LIB_AUX_TOOLS_H

#include <cstring>

#include "my_types.h"

#define MAX_VAL 1000000 // Max value used to calculate rand_prob()
#define MAX_DBL 100.0   // Max generated value by rand_value()

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