#include "database.hpp"

database::database(word name) {
	this->next_id = -1;
	this->last_file_id = 0;
	this->name = name;
	this->seed = time(NULL);

	if(!this->check_dir()) {
		message("[DB]: No database found!");
		this->available = false;
	} else {
		message("[DB]: Database found!");
		this->available = true;

		if(this->load_metadata())
			message("[DB]: Metadata read succesfully!");
		else {
			message("[DB]: Couldn't read metadata!");
			this->available = false;
		}
	}
}

database::~database() {
	if(this->available) {
		message("[DB]: Closing database!");

		bool saved = this->save_metadata();
		this->table.clear();
		this->disk.clear();
		this->available = false;
		
		if(saved)
			message("[DB]: Metadata saved succesfully!");
		else {
			message("[DB]: Couldn't save metadata!");
			exit(EXIT_FAILURE);
		}
	}
}

bool database::create() {
	if(!this->check_dir()) {
		if(!this->create_dir()) {
			message("[DB]: Couldn't create database!");
			this->available = false;
			return false;
		} 
	} else {
		message("[DB]: Couldn't create database! It's already present!");
		this->available = false;
		return false;
	}

	this->available = true;

	word data_path = this->name + ccp_to_str("/") + ccp_to_str(DATA_NAME) + std::to_string(this->last_file_id) + ccp_to_str(EXT);
	this->disk.push_back(file {this->last_file_id, 0, -1});

	this->save_metadata();
	this->save_data(data_path);

	message("[DB]: Database created!");
	return true;
}

bool database::create_dummy(int rows) {
	if(!this->create())
		return false;

	for(int i = 0; i < rows; i++) {
		word rand_s = rand_string(STRING_SIZE, &this->seed);
		double rand_v = rand_value(&this->seed);

		this->insert(rand_s, rand_v);
	}

	return true;
}

// write_request = true;
// sem_wait(&database); // down on database's semaphore
bool database::insert(word name, double value){
	if(!this->available) {
		message("[DB]: No database available!");
		return false;
	}

	file f;
	std::ostringstream oss;

	row tmp = { value, 0, name };
	word data_path = this->name + ccp_to_str("/") + ccp_to_str(DATA_NAME);

	oss << data_path << this->last_file_id << EXT;
	this->load_data(oss.str().c_str());
	tmp.id = this->next_id;

	// if the file is full, create a new one and add to the index
	if(this->table.size() == ROWS_PER_TABLE){
		this->table.clear();
		f.id = ++this->last_file_id;
		f.start_id = tmp.id;
		f.end_id = tmp.id;
		this->disk.push_back(f);
	}

	this->table.push_back(tmp);
	this->disk[this->last_file_id].end_id = tmp.id;

	oss.str("");
	oss.clear();
	oss << data_path << this->last_file_id << EXT;
	this->save_data(oss.str().c_str());
}
// usleep(FRAME_STEP); // wait
// sem_post(&database); // up on database's semaphore
// write_request = false;

bool database::is_available() {
	return this->available;
}

// lock_reader();
bool database::get_id(long int lower, long int upper, bool verbose){
	if(!this->available) {
		message("[DB]: No database available!");
		return false;
	}

	int matches = 0;
	std::ostringstream oss;
	word data_path = this->name + ccp_to_str("/") + ccp_to_str(DATA_NAME);

	if(verbose) {
		std::cout << "[DB]: Get by ID: " << lower << " to " << upper << std::endl;

		// printf("");
		message("ID  NAME  VALUE");
	}

	for(int i = 0; i < this->disk.size(); i++){
		oss << data_path << this->disk[i].id << EXT;
		
		if(!this->load_data(oss.str().c_str()))
			return false;
		
		for(int j = 0; j < table.size(); j++)
			if(this->table[j].id >= lower && this->table[j].id <= upper) {
				matches++;
				if(verbose)
					std::cout << this->table[j].id << " "
				              << this->table[j].name << " "
				              << this->table[j].value << std::endl;
			}
		
		oss.str("");
		oss.clear();
	}

	std::cout << "[DB]: " << matches << " matches found" << std::endl;

	return true;
}
// usleep(FRAME_STEP); // wait
// unlock_reader();

// lock_reader();
bool database::get_name(word lower, word upper, bool verbose){
	if(!this->available) {
		message("[DB]: No database available!");
		return false;
	}

	int matches = 0;
	std::ostringstream oss;
	word data_path = this->name + ccp_to_str("/") + ccp_to_str(DATA_NAME);

	if(verbose) {
		std::cout << "[DB]: Get by NAME: " << lower << " to " << upper << std::endl;
		// printf("");
		message("ID  NAME  VALUE");
	}

	for(int i = 0; i < this->disk.size(); i++){
		oss << data_path << this->disk[i].id << EXT;

		if(!this->load_data(oss.str().c_str()))
			return false;

		for(int j = 0; j < this->table.size(); j++)
			if(this->table[j].name.compare(lower) >= 0 && this->table[j].name.compare(upper) <= 0) {
				matches++;
				if(verbose)
					std::cout << this->table[j].id << " "
			        	      << this->table[j].name << " "
			            	  << this->table[j].value << std::endl;
			}

		oss.str("");
		oss.clear();
	}

	std::cout << "[DB]: " << matches << " matches found" << std::endl;

	return true;
}
// usleep(FRAME_STEP); // wait
// unlock_reader();

// lock_reader();
bool database::get_value(double lower, double upper, bool verbose){
	if(!this->available) {
		message("[DB]: No database available!");
		return false;
	}

	int matches = 0;
	std::ostringstream oss;
	word data_path = this->name + ccp_to_str("/") + ccp_to_str(DATA_NAME);

	if(verbose) {
		std::cout << "[DB]: Get by VALUE: " << lower << " to " << upper << std::endl;
		// printf("");
		message("ID  NAME  VALUE");
	}

	for(int i = 0; i < this->disk.size(); i++){
		oss << data_path << this->disk[i].id << EXT;

		if(!this->load_data(oss.str().c_str()))
			return false;

		for(int j = 0; j < this->table.size(); j++)
			if(this->table[j].value >= lower && this->table[j].value <= upper) {
				matches++;
				if(verbose)
					std::cout << this->table[j].id << " "
			        	      << this->table[j].name << " "
			            	  << this->table[j].value << std::endl;
			}

		oss.str("");
		oss.clear();
	}
	
	std::cout << "[DB]: " << matches << " matches found" << std::endl;

	return true;
}
// usleep(FRAME_STEP); // wait
// unlock_reader();

bool database::check_dir() {
	struct stat info;
	if(stat(this->name.c_str(), &info) == 0)
		return true;
	return false;
}

bool database::create_dir() {
	if(mkdir(this->name.c_str(), S_IRWXU | S_IRWXG | S_IRWXO) >= 0)
		return true;
	return false;
}

bool database::load_data(word file_name) {
	if(!this->available) {
		message("[DB]: No database available!");
		return false;
	}

	std::ifstream in_data(file_name.c_str());

	if(!in_data.is_open())
		return false;
	
	this->table = data();

	row tmp;
	while(in_data >> tmp.id >> tmp.name >> tmp.value){
		if(tmp.id > this->next_id)
			this->next_id = tmp.id;
		this->table.push_back(tmp);
	}

	this->next_id++;
	in_data.close();
	return true;
}

bool database::load_metadata() {
	if(!this->available) {
		message("[DB]: No database available!");
		return false;
	}

	std::ifstream in_data;
	word file_name = this->name + ccp_to_str("/") + ccp_to_str(META_NAME) + ccp_to_str(EXT);

	in_data.open(file_name.c_str());
	if(!in_data.is_open())
		return false;

	this->disk = files();

	file tmp;
	while(in_data >> tmp.id >> tmp.start_id >> tmp.end_id){
		if(tmp.id > this->last_file_id)
			this->last_file_id = tmp.id;
		this->disk.push_back(tmp);
	}

	in_data.close();
	return true;
}

bool database::save_metadata() {
	if(!this->available) {
		message("[DB]: No database available!");
		return false;
	}

	std::ofstream out_data;
	word file_name = this->name + ccp_to_str("/") + ccp_to_str(META_NAME) + ccp_to_str(EXT);

	out_data.open(file_name.c_str());
	if(!out_data.is_open())
		return false;

	for (int i = 0; i < this->disk.size(); i++)
		out_data << this->disk[i].id << " "
	             << this->disk[i].start_id << " "
	             << this->disk[i].end_id << std::endl;

	out_data.close();
	return true;
}

bool database::save_data(word file_name) {
	if(!this->available) {
		message("[DB]: No database available!");
		return false;
	}

	std::ofstream out_data;
	
	out_data.open(file_name.c_str());
	if(!out_data.is_open())
		return false;
	
	for (int i = 0; i < this->table.size(); i++)
		out_data << this->table[i].id << " "
	             << this->table[i].name << " "
	             << this->table[i].value << std::endl;

	out_data.close();
	return true;
}
