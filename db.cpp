#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <stdlib.h>
#include <pthread.h>

// Data format to be stored in the DB
class row {
	public:
		long id;
		std::string name;
		double value;
};

// Mutex for synchronizing the operations
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
// Stores the next available id to be used when creating a new row
long next_id = 0;
// Data representation in memory
std::vector<row> table;

void insert(std::string name, double value){
	row tmp;
	// Takes the lock to ensure no two entries have the same id
	pthread_mutex_lock(&mutex);
	tmp.id = next_id++;
	pthread_mutex_unlock(&mutex);
	tmp.name = name;
	tmp.value = value;
	pthread_mutex_lock(&mutex);
	table.push_back(tmp);
	pthread_mutex_unlock(&mutex);
}

void get_id(long lower, long upper){
}

void get_name(std::string lower, std::string upper){
}

void get_value(double lower, double upper){
}

// Load the data into memory
bool read_data(const char *file_name) {
	std::ifstream in_data;
	in_data.open(file_name);
	if(!in_data.is_open())
		return false;
	table = std::vector<row>();
	row tmp;
	while(in_data >> tmp.id >> tmp.name >> tmp.value){
		if(tmp.id > next_id)
			next_id = tmp.id;
		table.push_back(tmp);
	}
	next_id++;
	in_data.close();
	return true;
}

// Write the data to the disk
bool save_data(const char *file_name) {
	std::ofstream out_data;
	out_data.open(file_name);
	if(!out_data.is_open())
		return false;
	for (int i = 0; i < table.size(); i++)
		out_data << table[i].id << " " << table[i].name << " " << table[i].value << std::endl;
	out_data.close();
	return true;
}

int main(int argc, char* argv[]){
	// The data file stores the data when the system is not running
	// As this is a very simplified version of a DB, we are not going
	// to constantly write to it

	if(read_data("data.txt"))
		std::cout << "Data read succesfully!" << std::endl;
	else {
		std::cout << "Couldn't read data!" << std::endl;
		exit(EXIT_FAILURE);
	}

	if(save_data("data.txt"))
		std::cout << "Data saved succesfully!" << std::endl;
	else {
		std::cout << "Couldn't save data!" << std::endl;
		exit(EXIT_FAILURE);
	}

	return 0;
}
