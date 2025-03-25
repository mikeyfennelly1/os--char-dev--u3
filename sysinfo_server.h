#ifndef SERVER_H
#define SERVER_H

#include <pthread.h>

typedef struct ThreadPool {
    pthread_t* workers;
    int num_workers;
} ThreadPool;

/**
 * @brief - Create a thread pool of size 'num_workers'.
 * 
 * @param num_workers - the number of worker threads in the thread pool.
 * 
 * @return integer status code.
 */
ThreadPool* create_worker_pool(int num_workers);

void wait_on_thread_pool(ThreadPool* pool);

int create_server_and_listen_on_port(void);

#endif