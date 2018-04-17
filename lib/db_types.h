#ifndef LIB_DB_TYPES
#define LIB_DB_TYPES

// Data format to be stored in the DB
typedef struct {
	double value;
	long id;
	std::string name;
} row;

// Data format of a storage file
typedef struct {
	long id;
	long start_id;
	long end_id;
} file;

// Arguments to be passed to threads
typedef struct {
	unsigned int id;
	unsigned int seed;
} thread_args;

#endif