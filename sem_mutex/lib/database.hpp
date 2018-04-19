#ifndef LIB_DATABASE_HPP
#define LIB_DATABASE_HPP

#include <fstream>
#include <sstream>

#include <sys/stat.h>

#include "aux_tools.h"
#include "my_types.h"

#define DATA_NAME "data"
#define META_NAME "index"
#define EXT       ".dat"

#define ROWS_PER_TABLE 100
#define STRING_SIZE 20

class database {
	public:
		database(word name);
		~database();

		bool create(); // Create database
		bool create_dummy(int rows); // Creates dummy database for test purposes
		bool insert(word name, double value); // Inserts element {name, value}
		bool is_available(); // Checks if database is available

		// Get methods. If 'verbose' is true, show matches
		bool get_id(long int lower, long int upper, bool verbose = true);
		bool get_name(word lower, word upper, bool verbose = true);
		bool get_value(double lower, double upper, bool verbose = true);

	private:
		data table; // Data representation in memory
		files disk; // Files presents in the disk
		long int last_file_id, // Last present table ID
		         next_id; // Next available ID to be used when creating a new row in 'table'
		word name;  // Database dir name
		unsigned int seed; // For creating dummy database
		bool available;

		bool check_dir();  // Checks if database dir exists
		bool create_dir(); // Creates database dir
		bool load_data(word file_name);  // Loads disk data into memory
		bool load_metadata(); // Loads metadata for database recovery from disk
		bool save_data(word file_name);  // Saves memory data into disk
		bool save_metadata(); // Saves metadata for database recovery from disk
};

#endif
