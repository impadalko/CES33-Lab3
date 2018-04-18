#include <cstring>
#include <iostream>
#include <vector>

#include <pthread.h>
#include <semaphore.h>
#include <stdlib.h>
#include <unistd.h> // usleep
#include <time.h>

#include "lib/aux_tools.h"
#include "lib/db_types.h"
#include "lib/database.hpp"

#define N_READERS 3
#define N_FRAMES 3
#define FRAME_STEP (int) (1.0 * 1e6) // (int) (SECONDS * 1e6) MICROSECONDS

#define ACTION_WRITE     1

#define ACTION_WAIT      0 
#define ACTION_GET_ID    1
#define ACTION_GET_NAME  2
#define ACTION_GET_VALUE 3
#define POSSIBLE_ACTIONS 4

#define DB_NAME "database"

// thread stuff
sem_t mutex;     // Mutex semaphore
sem_t db_access; // Database semaphore
sem_t show;      // For printing output
bool write_request = false;
unsigned int num_of_readers = 0;
pthread_t reader_id[N_READERS]; // ID of threads/readers
pthread_t writer_id;            // ID of thread/writer

database db(DB_NAME);

void *reader_func(void *_args) {
	thread_args *args = (thread_args *) _args;
	
	unsigned int id = args->id,
	             seed = args->seed;

	sem_wait(&show);
	std::cout << "reader " << id << " is ONLINE\n";
	sem_post(&show);

	for(int i = 0; i < N_FRAMES; i++) {
		word str;
		int action = (int) (rand_prob(&seed) * POSSIBLE_ACTIONS);

		switch(action) {
			case ACTION_WAIT:
				sem_wait(&show);
				message("reader " + std::to_string(id) + " will wait");
				sem_post(&show);

				usleep(FRAME_STEP);
				break;

			case ACTION_GET_ID:
				sem_wait(&show);
				message("reader " + std::to_string(id) + " will get element by id");
				sem_post(&show);

				usleep(FRAME_STEP);
				// get_id(990, 1010);
				break;

			case ACTION_GET_NAME:
				sem_wait(&show);
				message("reader " + std::to_string(id) + " will get element by name");
				sem_post(&show);

				usleep(FRAME_STEP);
				// get_name("a", "z");
				break;

			case ACTION_GET_VALUE:
				sem_wait(&show);
				message("reader " + std::to_string(id) + " will get element by value");
				sem_post(&show);

				usleep(FRAME_STEP);
				// get_value(0.5, 1.5);
				break;

			default:
				break;
		}

		// sem_wait(&mutex); // down no semaforo
		// std::cout << id << " accessing\n";
		// std::cout << "generate " << rand_prob(&seed) << std::endl;
		// std::cout << std::endl;
		// sem_post(&mutex); // up no semaforo
	}

	sem_wait(&show);
	std::cout << "reader " << id << " is OFFLINE\n";
	sem_post(&show);

	return NULL;
}

// TODO: implement this function
void *writer_func(void *_args) {
	thread_args *args = (thread_args *) _args;
	
	unsigned int id = args->id,
	             seed = args->seed;

	// for(int i = 0; i < N_FRAMES; i++) {
	// 	sem_wait(&mutex); // down no semaforo
	// 	std::cout << id << " accessing" << std::endl;
	// 	std::cout << "generate " << rand_prob(&seed) << std::endl;
	// 	std::cout << std::endl;
	// 	sem_post(&mutex); // up no semaforo
	// }

	return NULL;
}

void lock_reader() {
	while(write_request) { } // wait until current writing finish

	sem_wait(&mutex);
	num_of_readers++;
	if(num_of_readers == 1)
		sem_wait(&db_access);
	sem_post(&mutex);
}

void unlock_reader() {
	sem_wait(&mutex);
	num_of_readers--;
	if(num_of_readers == 0)
		sem_post(&db_access);
	sem_post(&mutex);
}

void thread_test() {
	thread_args reader_args[N_READERS],
	            writer_args;

	srand(time(NULL)); // to generate seeds	
	for(int i = 0; i < N_READERS ; i++) // generate seeds
		reader_args[i].seed = rand();
	writer_args.seed = rand();

	for(int i = 0; i < N_READERS; i++) { // cria as threads
		reader_args[i].id = i;
		pthread_create (&reader_id[i], // id
			            NULL,          // 
			            reader_func,   // funçao da thread
			            (void *) &reader_args[i]); // argumento
	}

	writer_args.id = N_READERS; // writer has the last ID
	pthread_create(&writer_id,
		           NULL,
		           writer_func,
		           (void *) &writer_args);

	for(int i = 0; i < N_READERS; i++) // wait all threads to finish
 		pthread_join(reader_id[i], NULL);
 	pthread_join(writer_id, NULL);
}

int main(int argc, char* argv[]) {
	// sem_init(&mutex, 0, 1);    // intialize mutex
	// sem_init(&db_access, 0, 1); // initialize database semaphore
	// sem_init(&show, 0, 1);     // initialize semaphore
	
	// db.create_dummy(10);
	if(!db.is_available())
		exit(EXIT_FAILURE);

	if(!db.get_value(50.0, 80.0))
		exit(EXIT_FAILURE);

	// thread_test();

	return EXIT_SUCCESS;
}

