#ifndef LIB_AUX_TOOLS
#define LIB_AUX_TOOLS

// Shows string 'str' in 'stdout'
void message(std::string str);

// Converts "const char *" to "std::string"
std::string ccp_to_str(const char *str);

// Returns a random number in [0.0, 1.0] interval
double rand_prob(unsigned int *seed);

#endif