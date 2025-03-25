#ifndef SERVER_H
#define SERVER_H

#include <pthread.h>

typedef struct WorkerPool {
    pthread_t* workers;
    int num_workers;
} WorkerPool;

/**
 * @brief - Create a thread pool of size 'num_workers'.
 * 
 * @param num_workers - the number of worker threads in the thread pool.
 * 
 * @return integer status code.
 */
WorkerPool* create_worker_pool(int num_workers);

void wait_on_worker_pool(WorkerPool* pool);

int start_server(void);

#endif