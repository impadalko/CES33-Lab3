#ifndef LIB_DB_TYPES
#define LIB_DB_TYPES

#include <iostream>
#include <cstring>
#include <vector>

typedef std::string word;

// Data format to be stored in the DB
typedef struct {
	double value;
	long int id;
	word name;
} row;

// Data format of a storage file
typedef struct {
	long int id;
	long int start_id;
	long int end_id;
} file;

// Arguments to be passed to threads
typedef struct {
	unsigned int id;
	unsigned int seed;
} thread_args;

typedef std::vector<row>  data;
typedef std::vector<file> files;

#endif
