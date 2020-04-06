#ifndef GLOBALS
#define GLOBALS
#include <pthread.h>
#include "hashset.h"
#ifdef  MAIN_FILE
//int currkey;
hashset_t keys;
pthread_mutex_t mutex;
#else
//extern int currkey;
extern hashset_t keys;
extern pthread_mutex_t mutex;
#endif
#endif