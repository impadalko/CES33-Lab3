#include <cstring>
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>

#include <pthread.h>
#include <semaphore.h>
#include <stdlib.h>
#include <unistd.h> // usleep

#include "lib/aux_tools.h"
#include "lib/db_types.h"
#include "lib/db_tools.h"

#define N_READERS 3
#define N_FRAMES 3

#define DATABASE "database/"
#define DATAFILE "data"
#define INDEX    "index"
#define EXT      ".txt"

sem_t mutex;    // Mutex semaphore
sem_t database; // Database semaphore
// sem_t request;  // For writing data

long next_id = 0;   // Stores the next available id to be used when creating a new row
long last_file_id = 0;
long file_size = 1000; // Size of one storage file
unsigned int num_of_readers = 0;
std::vector<row> table; // Data representation in memory
std::vector<file> disk; // Files presents in the disk

// pthread_t reader_id[N_READERS]; // ID of threads/readers
// pthread_t writer_id;            // ID of thread/writer

// typedef struct {
// 	unsigned int id, seed;
// } thread_args;

// void *thread_func(void *arg) {
// 	thread_args *args = (thread_args *) arg;
	
// 	unsigned int id = args->id,
// 	             seed = args->seed;

// 	for(int i = 0; i < N_FRAMES; i++) {
// 		sem_wait(&mutex); // down no semaforo
// 		std::cout << id << " accessing" << std::endl;
// 		std::cout << "generate " << rand_r(&seed) % 100 << std::endl;
// 		std::cout << std::endl;
// 		sem_post(&mutex); // up no semaforo
// 	}

// 	return 0;
// }
 	
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

// TODO: make request to insert. after request, no new readers are allowed
void insert(std::string name, double value){
	file f;
	std::ostringstream oss;

	row tmp = { 0, name, value };
	std::string data_path = ccp_to_str(DATABASE)+ccp_to_str(DATAFILE);

	sem_wait(&database); // down on database's semaphore

	oss << data_path << last_file_id << EXT;
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
	oss << data_path << last_file_id << EXT;
	save_data(oss.str().c_str());

	sem_post(&database); // up on database's semaphore
}

void get_id(long lower, long upper){
	std::ostringstream oss;
	std::string data_path = ccp_to_str(DATABASE)+ccp_to_str(DATAFILE);

	lock_reader();

	message("ID  NAME  VALUE");
	for(int i = 0; i < disk.size(); i++){
		oss << data_path << disk[i].id << EXT;
		
		if(read_data(oss.str().c_str()))
			message("Data read succesfully!");
		else {
			message("Couldn't read data!");
			exit(EXIT_FAILURE);
		}
		
		for(int j = 0; j < table.size(); j++)
			if(table[j].id >= lower && table[j].id <= upper)
				std::cout << table[j].id << " " << table[j].name << " " << table[j].value << std::endl;
		
		oss.str("");
		oss.clear();
	}

	unlock_reader();
}

void get_name(std::string lower, std::string upper){
	std::ostringstream oss;
	std::string data_path = ccp_to_str(DATABASE)+ccp_to_str(DATAFILE);

	lock_reader();

	message("ID  NAME  VALUE");
	for(int i = 0; i < disk.size(); i++){
		oss << data_path << disk[i].id << EXT;

		if(read_data(oss.str().c_str()))
			message("Data read succesfully!");
		else {
			message("Couldn't read data!");
			exit(EXIT_FAILURE);
		}

		for(int j = 0; j < table.size(); j++)
			if(table[j].name.compare(lower) >= 0 && table[j].name.compare(upper) <= 0)
				std::cout << table[j].id << " " << table[j].name << " " << table[j].value << std::endl;

		oss.str("");
		oss.clear();
	}

	unlock_reader();
}

void get_value(double lower, double upper){
	std::ostringstream oss;
	std::string data_path = ccp_to_str(DATABASE)+ccp_to_str(DATAFILE);

	lock_reader();

	message("ID  NAME  VALUE");
	for(int i = 0; i < disk.size(); i++){
		oss << data_path << disk[i].id << EXT;

		if(read_data(oss.str().c_str()))
			message("Data read succesfully!");
		else {
			message("Couldn't read data!");
			exit(EXIT_FAILURE);
		}

		for(int j = 0; j < table.size(); j++)
			if(table[j].value >= lower && table[j].value <= upper)
				std::cout << table[j].id << " " << table[j].name << " " << table[j].value << std::endl;

		oss.str("");
		oss.clear();
	}

	unlock_reader();
}

// void bizu_das_threads() {
// 	thread_args reader_args[N_READERS],
// 	            writer_args;

// 	srand(time(NULL)); // to generate seeds	
// 	for(int i = 0; i < N_READERS ; i++) // generate seeds
// 		reader_args[i].seed = rand();
// 	writer_args.seed = rand();

// 	for(int i = 0; i < N_READERS; i++) { // cria as threads
// 		reader_args[i].id = i;
// 		pthread_create (&reader_id[i], // id
// 			            NULL,          // ?
// 			            thread_func,   // funçao da thread
// 			            (void *) &reader_args[i]); // argumento
// 	}

// 	writer_args.id = N_READERS;
// 	pthread_create(&writer_id,
// 		           NULL,
// 		           thread_func,
// 		           (void *) &writer_args);

// 	for(int i = 0; i < N_READERS; i++) // wait all threads to finish
//  		pthread_join(reader_id[i], NULL);
//  	pthread_join(writer_id, NULL);
// }

int main(int argc, char* argv[]) {
	std::string index_path = ccp_to_str(DATABASE)+ccp_to_str(INDEX)+ccp_to_str(EXT);

	sem_init(&mutex, 0, 1);    // intialize mutex
	sem_init(&database, 0, 1); // initialize database semaphore

	if(!check_database(DATABASE)) {
		message("Creating database!");
		if(create_database(DATABASE))
			message("Database created!");
		else {
			message("Couldn't create database!");
			exit(EXIT_FAILURE);
		}
	}

	if(read_disk(index_path))
		message("Index read succesfully!");
	else {
		message("Couldn't read index!");
		exit(EXIT_FAILURE);
	}

	// For generate DB
	// for (int i = 0; i < 2000; i++)
	// 	insert("a", 1);

	// ID test
	// get_id(990, 1010);

	// NAME test
	// get_name("a", "z");

	// VALUE test
	// get_value(1, 2);

	if(write_disk(index_path))
		message("Index wrote succesfully!");
	else {
		message("Couldn't write index!");
		exit(EXIT_FAILURE);
	}

	return EXIT_SUCCESS;
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

// Write data to disk
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
