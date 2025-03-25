#ifndef SERVER_H
#define SERVER_H

#include <pthread.h>

/**
 * @brief - Create a thread pool of size 'num_workers'.
 * 
 * @param num_workers - the number of worker threads in the thread pool.
 * 
 * @return integer status code.
 */
int create_worker_pool(int num_workers);

typedef struct ThreadPool {
    pthread_t* workers;
    int num_workers;
} ThreadPool;

void wait_on_thread_pool(ThreadPool* pool);

#endif