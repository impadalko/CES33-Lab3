#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#include "lib/aux_tools.h"
#include "lib/my_types.h"

#include "lib/database.hpp"

// ----------------------------------------------------------------------------

#define N_READERS 3    // Number of readers
#define READER_STEPS 3 // Number of reader actions
#define WRITER_STEPS 7 // Number of writer actions
#define FRAME_STEP (int) (0.1 * 1e6) // (int) (SECONDS * 1e6) MICROSECONDS
#define MAX_DELAY    5 // Maximum number of FRAME_STEPS in a delay

#define ACTION_WRITE     0 // Makes writer write
#define WRITER_ACTIONS   1 // Number of possible writer actions
#define WRITER_STRING_SIZE 10 // Size of new string to be written by writer

#define ACTION_GET_ID    0 // Makes reader get elements by id
#define ACTION_GET_NAME  1 // Makes reader get elements by name
#define ACTION_GET_VALUE 2 // Makes reader get elements by value
#define READER_ACTIONS   3 // Number of possible reader actions

#define DB_NAME "database" // Database name

// ----------------------------------------------------------------------------

sem_t mutex;     // Mutex semaphore
sem_t db_access; // Database access semaphore
sem_t request;   // Writer semaphore

bool write_request = false; // 'true' when writer is ready to write
unsigned int num_of_readers = 0; // Number of readers reading

database db(DB_NAME); // The Database

// ----------------------------------------------------------------------------

void lock_reader(int id);      // Increment number of readers
void unlock_reader(int id);    // Decrement number of readers
void reader_waiting(int id);   // Loop for reader to wait writer finish to write
void *reader_func(void *args); // Reader thread function/behavior
void *writer_func(void *args); // Writer thread function/behavior

// ----------------------------------------------------------------------------

int main(int argc, char* argv[]) {
	// Checking database ------------------------------------------------------
	// db.create_dummy(100);
	if(!db.is_available())
		exit(EXIT_FAILURE);
	// ------------------------------------------------------------------------

	// Initializing synchronizers ---------------------------------------------
	pthread_t reader_id[N_READERS]; // ID of readers
	pthread_t writer_id;            // ID of writer

	thread_args reader_args[N_READERS], // arguments of readers
	            writer_args;            // arguments of writer

	sem_init(&mutex, 0, 1);         // intialize mutex
	sem_init(&db_access, 0, 1);     // initialize database semaphore
	sem_init(&request, 0, 1);     //
	// ------------------------------------------------------------------------

	// Generating seeds -------------------------------------------------------
	srand(time(NULL));                  //
	for(int i = 0; i < N_READERS ; i++) // generating seeds
		reader_args[i].seed = rand();   //
	writer_args.seed = rand();          //
	// ------------------------------------------------------------------------
	
	// Creating threads -------------------------------------------------------
	for(int i = 0; i < N_READERS; i++) {
		reader_args[i].id = i;
		pthread_create (&reader_id[i], // id
			            NULL,          // 
			            reader_func,   // thread function
			            (void *) &reader_args[i]); // argumento for thread function
	}

	writer_args.id = N_READERS; // writer has the last ID
	pthread_create(&writer_id,  // id
		           NULL,        //
		           writer_func, // thread function
		           (void *) &writer_args); // argument for thread function
	// ------------------------------------------------------------------------

	// Joining threads --------------------------------------------------------
	for(int i = 0; i < N_READERS; i++) // wait all threads to finish
 		pthread_join(reader_id[i], NULL);
 	pthread_join(writer_id, NULL);
	// ------------------------------------------------------------------------

	return EXIT_SUCCESS;
}

// ----------------------------------------------------------------------------

void lock_reader(int id) {
	sem_wait(&mutex);

	num_of_readers++;
	message("[READER >>]: " + std::to_string(id) + " locked db (" + std::to_string(num_of_readers) + " reading)");
	if(num_of_readers == 1)
		sem_wait(&db_access);

	sem_post(&mutex);
}

void unlock_reader(int id) {
	sem_wait(&mutex);

	num_of_readers--;	
	message("[READER >>]: " + std::to_string(id) + " unlocked db (" + std::to_string(num_of_readers) + " reading)");
	if(num_of_readers == 0)
		sem_post(&db_access);

	sem_post(&mutex);
}

void reader_waiting(int id) {
	// bool feedback = true;
	// while(true) {
	// 	sem_wait(&mutex);
	// 	if(!write_request)
	// 		break;
	// 	sem_post(&mutex);

	// 	if(feedback) {

	// sem_wait(&mutex);
	// message("[READER >>]: " + std::to_string(id) + " is waiting to read");
	// sem_post(&mutex);

	sem_wait(&request);
	sem_post(&request);

	// 		feedback = false;
	// 	}
	// } 	
}

void *reader_func(void *_args) {
	thread_args *args = (thread_args *) _args;
	
	unsigned int id = args->id,
	             seed = args->seed;

	sem_wait(&mutex);
	std::cout << "[READER >>]: " << id << " is ONLINE\n";
	sem_post(&mutex);

	for(int i = 0; i < READER_STEPS; i++) {
		int action = (int) (rand_prob(&seed) * READER_ACTIONS),
		    set_delay = (int) (rand_prob(&seed) * MAX_DELAY);

		reader_waiting(id);
		lock_reader(id);

		switch(action) {
			case ACTION_GET_ID:
				sem_wait(&mutex);
				message("[READER >>]: " + std::to_string(id) + " will get element by id");
				sem_post(&mutex);
				// get_id(990, 1010);
				break;

			case ACTION_GET_NAME:
				sem_wait(&mutex);
				message("[READER >>]: " + std::to_string(id) + " will get element by name");
				sem_post(&mutex);

				// get_name("a", "z");
				break;

			case ACTION_GET_VALUE:
				sem_wait(&mutex);
				message("[READER >>]: " + std::to_string(id) + " will get element by value");
				sem_post(&mutex);

				// get_value(0.5, 1.5);
				break;

			default:
				break;
		}
		
		unlock_reader(id);
		usleep(set_delay);		
	}

	sem_wait(&mutex);
	std::cout << "[READER >>]: " << id << " is OFFLINE\n";
	sem_post(&mutex);

	return NULL;
}

void *writer_func(void *_args) {
	thread_args *args = (thread_args *) _args;
	
	unsigned int id = args->id,
	             seed = args->seed;

	sem_wait(&mutex);
	std::cout << "[<< WRITER]: " << id << " is ONLINE\n";
	sem_post(&mutex);

	for(int i = 0; i < WRITER_STEPS; i++) {
		int action = (int) (rand_prob(&seed) * WRITER_ACTIONS),
		    set_delay = (int) (rand_prob(&seed) * MAX_DELAY);

		double new_value = rand_value(&seed);
		word new_word = rand_string(WRITER_STRING_SIZE, &seed);
		
		switch(action) {
			case ACTION_WRITE:
				sem_wait(&request);

				sem_wait(&mutex);
				// write_request = true;
				message("[<< WRITER]: " + std::to_string(id) + " locked db for new readers (" + std::to_string(num_of_readers) + " reading)");
				sem_post(&mutex);

				// sem_wait(&mutex);
				// message("[<< WRITER]: " + std::to_string(id) + " inserting");
				// sem_post(&mutex);				

				sem_wait(&db_access);
				db.insert(new_word, new_value);
				sem_post(&db_access);

				sem_wait(&mutex);
				message("[<< WRITER]: " + std::to_string(id) + " unlocked db");
				// write_request = false;
				sem_post(&mutex);

				sem_post(&request);

				usleep(set_delay);
				break;

			default:
				break;
		}
	}

	sem_wait(&mutex);
	std::cout << "[<< WRITER]: " << id << " is OFFLINE\n";
	sem_post(&mutex);

	return NULL;
}
