#include "aux_tools.h"

void message(word str) {
	std::cout << str << std::endl;
}

word ccp_to_str(const char *str) {
	return word(str, strlen(str));
}

double rand_prob(unsigned int *seed) {
	int res = rand_r(seed) % MAX_VAL;
	return ((double) res)/MAX_VAL;
}

word rand_string(int size, unsigned int *seed) {
	word ret;

	for(int i = 0; i < size; i++) {
		double chance = rand_prob(seed);

		if(chance < 0.85)
			ret += 'a' + (int) (rand_prob(seed) * 25);
		else
			ret += 'A' + (int) (rand_prob(seed) * 25);
	}

	return ret;
}

double rand_value(unsigned int *seed) {
	return rand_prob(seed) * MAX_DBL;
}
