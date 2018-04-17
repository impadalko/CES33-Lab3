#ifndef LIB_DB_TYPES
#define LIB_DB_TYPES

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

#endif