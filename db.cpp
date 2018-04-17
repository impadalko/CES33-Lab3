#include <cstring>
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>
#include <sys/types.h>
#include <sys/stat.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdlib.h>

#define DATABASE "database/"
#define DATAFILE "data"
#define INDEX    "index"
#define EXT      ".txt"

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

sem_t mutex;    // Mutex semaphore
sem_t database; // Database semaphore

long next_id = 0;   // Stores the next available id to be used when creating a new row
long last_file_id = 0;
long file_size = 1000; // Size of one storage file
unsigned int num_of_readers = 0;
std::vector<row> table; // Data representation in memory
std::vector<file> disk; // Files presents in the disk

std::string ccp_to_str(const char *str); // "const char *" to "std::string"
bool check_database();
bool create_database();
bool read_disk(std::string file_name);
bool write_disk(std::string file_name);
bool read_data(std::string file_name);
bool save_data(std::string file_name);

void lock_reader() {
	sem_wait(&mutex);
	num_of_readers++;
	if(num_of_readers == 1)
		sem_wait(&database);
	sem_post(&mutex);
}

void unlock_reader() {
	sem_wait(&mutex);
	num_of_readers--;
	if(num_of_readers == 0)
		sem_post(&database);
	sem_post(&mutex);
}

void insert(std::string name, double value){
	row tmp = { 0, name, value };
	sem_wait(&database); // sem_wait on database's semaphore
	file f;
	std::ostringstream oss;
	oss << ccp_to_str(DATABASE) + ccp_to_str(DATAFILE) << last_file_id << EXT;
	read_data(oss.str().c_str());
	tmp.id = next_id;
	// if the file is full, create a new one and add to the index
	if(table.size() == file_size){
		table.clear();
		f.id = ++last_file_id;
		f.start_id = tmp.id;
		f.end_id = tmp.id;
		disk.push_back(f);
	}
	table.push_back(tmp);
	disk[last_file_id].end_id = tmp.id;
	oss.str("");
	oss.clear();
	oss << ccp_to_str(DATABASE) + ccp_to_str(DATAFILE) << last_file_id << EXT;
	save_data(oss.str().c_str());
	sem_post(&database); // up on database's semaphore
}

void get_id(long lower, long upper){
	lock_reader();

	std::ostringstream oss;
	std::cout << "ID  NAME  VALUE" << std::endl;
	for(int i = 0; i < disk.size(); i++){
		oss << ccp_to_str(DATABASE) + ccp_to_str(DATAFILE) << disk[i].id << EXT;
		if(read_data(oss.str().c_str()))
			std::cout << "Data read succesfully!" << std::endl;
		else {
			std::cout << "Couldn't read data!" << std::endl;
			exit(EXIT_FAILURE);
		}
		for(int j = 0; j < table.size(); j++)
			if(table[i].id >= lower && table[i].id <= upper)
				std::cout << table[j].id << " " << table[j].name << " " << table[j].value << std::endl;
		oss.str("");
		oss.clear();
	}

	unlock_reader();
}

void get_name(std::string lower, std::string upper){
	lock_reader();

	std::ostringstream oss;
	std::cout << "ID  NAME  VALUE" << std::endl;
	for(int i = 0; i < disk.size(); i++){
		oss << ccp_to_str(DATABASE) + ccp_to_str(DATAFILE) << disk[i].id << EXT;
		if(read_data(oss.str().c_str()))
			std::cout << "Data read succesfully!" << std::endl;
		else {
			std::cout << "Couldn't read data!" << std::endl;
			exit(EXIT_FAILURE);
		}
		for(int j = 0; j < table.size(); j++)
			if(table[i].name.compare(lower) >= 0 && table[i].name.compare(upper) <= 0)
				std::cout << table[j].id << " " << table[j].name << " " << table[j].value << std::endl;
		oss.str("");
		oss.clear();
	}

	unlock_reader();
}

void get_value(double lower, double upper){
	lock_reader();

	std::ostringstream oss;
	std::cout << "ID  NAME  VALUE" << std::endl;
	for(int i = 0; i < disk.size(); i++){
		oss << ccp_to_str(DATABASE) + ccp_to_str(DATAFILE) << disk[i].id << EXT;
		if(read_data(oss.str().c_str()))
			std::cout << "Data read succesfully!" << std::endl;
		else {
			std::cout << "Couldn't read data!" << std::endl;
			exit(EXIT_FAILURE);
		}
		for(int j = 0; j < table.size(); j++)
			if(table[i].value >= lower && table[i].value <= upper)
				std::cout << table[j].id << " " << table[j].name << " " << table[j].value << std::endl;
		oss.str("");
		oss.clear();
	}

	unlock_reader();
}

int main(int argc, char* argv[]) {
	sem_init(&mutex, 0, 1);    // intialize mutex
	sem_init(&database, 0, 1); // initialize database semaphore

	if(!check_database()) {
		std::cout << "Creating database!" << std::endl;
		if(create_database())
			std::cout << "Database created!" << std::endl;
		else {
			std::cout << "Couldn't create database!" << std::endl;
			exit(EXIT_FAILURE);
		}
	}

	if(read_disk(ccp_to_str(DATABASE) + ccp_to_str(INDEX) + ccp_to_str(EXT)))
		std::cout << "Index read succesfully!" << std::endl;
	else {
		std::cout << "Couldn't read index!" << std::endl;
		exit(EXIT_FAILURE);
	}

	for (int i = 0; i < 2000; i++)
		insert("a", 1);

	get_id(500, 1500);

	if(write_disk(ccp_to_str(DATABASE) + ccp_to_str(INDEX) + ccp_to_str(EXT)))
		std::cout << "Index wrote succesfully!" << std::endl;
	else {
		std::cout << "Couldn't write index!" << std::endl;
		exit(EXIT_FAILURE);
	}

	return 0;
}

std::string ccp_to_str(const char *str) {
	return std::string(str, strlen(str));
}

// Checks if database dir exists
bool check_database() {
	struct stat info;
	if(stat(DATABASE, &info) == 0)
		return true;
	return false;
}

bool create_database() {
	if(mkdir(DATABASE, S_IRWXU | S_IRWXG | S_IRWXO) >= 0)
		return true;
	return false;
}

// Load disk info to memory
bool read_disk(std::string file_name) {
	std::ifstream in_data;
	in_data.open(file_name.c_str());
	if(!in_data.is_open())
		return false;
	disk = std::vector<file>();
	file tmp;
	while(in_data >> tmp.id >> tmp.start_id >> tmp.end_id){
		if(tmp.id > last_file_id)
			last_file_id = tmp.id;
		disk.push_back(tmp);
	}
	last_file_id; // ???
	in_data.close();
	return true;
}

// Load the data into memory
bool read_data(std::string file_name) {
	std::ifstream in_data;
	in_data.open(file_name.c_str());

	// std::cout << file_name << std::endl;

	if(!in_data.is_open())
		return false;

	// std::cout << "is opened" << std::endl;

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

bool write_disk(std::string file_name) {
	std::ofstream out_data;
	out_data.open(file_name.c_str());
	if(!out_data.is_open())
		return false;
	for (int i = 0; i < disk.size(); i++)
		out_data << disk[i].id << " " << disk[i].start_id << " " << disk[i].end_id << std::endl;
	out_data.close();
	return true;
}

// Write the data to the disk
bool save_data(std::string file_name) {
	std::ofstream out_data;
	out_data.open(file_name.c_str());
	if(!out_data.is_open())
		return false;
	for (int i = 0; i < table.size(); i++)
		out_data << table[i].id << " " << table[i].name << " " << table[i].value << std::endl;
	out_data.close();
	return true;
}
