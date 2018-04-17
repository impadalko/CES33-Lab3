#include <cstring>
#include <iostream>
#include <stdlib.h>

#include "aux_tools.h"

#define MAX_VAL 1000000

void message(std::string str) {
	std::cout << str << std::endl;
}

std::string ccp_to_str(const char *str) {
	return std::string(str, strlen(str));
}

double rand_prob(unsigned int *seed) {
	int res = rand_r(seed) % MAX_VAL;
	return ((double) res)/MAX_VAL;
}