#include "global.hpp"

int clientNum = 0;
extern vector<managementTable> table(MAX_CLIENTS); // Definido apenas aqui
pthread_mutex_t global_lock = PTHREAD_MUTEX_INITIALIZER;
