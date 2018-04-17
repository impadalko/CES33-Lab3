#include <cstring>
#include <iostream>
#include "db_lib.h"

void message(std::string str) {
	std::cout << str << std::endl;
}

std::string ccp_to_str(const char *str) {
	return std::string(str, strlen(str));
}
