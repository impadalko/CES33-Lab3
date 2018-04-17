#ifndef DB_LIB
#define DB_LIB

// Data format to be stored in the DB
typedef struct {
	long id;
	std::string name;
	double value;
} row;

// Data format of a storage file
typedef struct {
	long id;
	long start_id;
	long end_id;
} file;

// Shows string 'str' in 'stdout'
void message(std::string str);

// Converts "const char *" to "std::string"
std::string ccp_to_str(const char *str);

#endif