#ifndef LIB_DB_TOOLS
#define LIB_DB_TOOLS

#include <cstring>
#include <iostream>

#include <sys/stat.h>

// Checks if database dir exists
bool check_database(std::string file_name);

// Creates database
bool create_database(std::string file_name);

#endif