#include "db_tools.h"

bool check_database(std::string file_name) {
	struct stat info;
	if(stat(file_name.c_str(), &info) == 0)
		return true;
	return false;
}

bool create_database(std::string file_name) {
	if(mkdir(file_name.c_str(), S_IRWXU | S_IRWXG | S_IRWXO) >= 0)
		return true;
	return false;
}
