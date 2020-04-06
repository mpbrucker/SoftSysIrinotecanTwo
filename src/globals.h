#ifndef GLOBALS
#define GLOBALS
#include <pthread.h>
#ifdef  MAIN_FILE
int currkey;
pthread_mutex_t mutex;
#else
extern int currkey;
extern pthread_mutex_t mutex;
#endif
#endif