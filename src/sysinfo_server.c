/**
 * sysinfo_server.c
 * 
 * A multithreaded server to expose a REST API to get sysinfo data.
 * This server exhibits the thread pool pattern.
 * 
 * @author Mikey Fennelly
 */

#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <stdbool.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#include "request_utils.h"
#include "sysinfo_server.h"

#define SERVER_PORT 8080        // TCP port to start server on
#define SERVER_BACKLOG 10       // number  of clients to queue before refusing connection requests
#define BUFSIZE 4096

SysinfoTask task_queue[256];    // task_queue to for thread pool
int task_count = 0;             // initial number of tasks on the queue set to 0

// sync variables
pthread_mutex_t mutex_queue;
pthread_cond_t cond_queue;
pthread_mutex_t mutex_get_sysinfo;

bool worker_pool_exists = false;

// function prototypes
void* start_thread(void* args);
void submit_task(SysinfoTask task);
void execute_task(SysinfoTask task);
int start_server(void);

/**
 * @brief - Create a thread pool of size 'num_workers'.
 * 
 * This function starts worker threads for worker pool of size num_workers.
 * It also initializes sync variables for internal use within the thread pool.
 * 
 * @param num_workers - the number of worker threads in the thread pool.
 * 
 * @return integer status code.
 */
WorkerPool*
create_worker_pool(int num_workers)
{
    // malloc a WorkerPool object
    WorkerPool* pool = (WorkerPool*) malloc(sizeof(WorkerPool));
    // malloc worker thread references and add them to WorkerPool as
    // the workers for this pool. 
    pool->workers = (pthread_t*) malloc(sizeof(pthread_t) * num_workers);
    // set num_workers in thread pool to equal num_workers
    pool->num_workers = num_workers;

    // initialize sync vars
    pthread_mutex_init(&mutex_queue, NULL);
    pthread_cond_init(&cond_queue, NULL);
    pthread_mutex_init(&mutex_get_sysinfo, NULL);

    // create worker threads for pool of size num_workers
    for (int i = 0; i < num_workers; i++)
    {
        if (pthread_create(&pool->workers[i], NULL, &start_thread, NULL) != 0)
        {
            perror("Failed to create thread\n");
        }
    }

    worker_pool_exists = true;

    return pool;
}

/**
 * Gracefully shut down a thread pool, given a WorkerPool object
 * 
 * @param pool - the thread pool to shut down
 */
void close_thread_pool(WorkerPool* pool)
{
    free(pool);
}

/**
 * @brief Block thread until all workers in WorkerPool have completed.
 */
void wait_on_worker_pool(WorkerPool* pool)
{
    for (int i = 0; i < pool->num_workers; i++)
    {
        if (pthread_join(pool->workers[i], NULL) != 0)
        {
            perror("Failed to join thread\n");
        };
    }

    // free memory occupied by pool
    free(pool);
}

/**
 * @brief Start a new worker thread.
 * 
 * This thread will wait for a signal to execute a task from the
 * task_queue.
 */
void*
start_thread(void* args)
{
    while (1)
    {
        bool found = false;
        
        // Lock the critical section where task is being taken from queue.
        pthread_mutex_lock(&mutex_queue);
        while (task_count == 0)
        {
            // wait for a signal to attempt to get an item from the queue
            pthread_cond_wait(&cond_queue, &mutex_queue);
        }
    
        found = true;
        SysinfoTask task;
        // pull first item from the queue
        task = task_queue[0];

        // shift all tasks in task_queue closer to the front of
        // queue by 1
        for (int i = 0; i < task_count - 1; i++)
        {
            task_queue[i] = task_queue[i + 1];
        }
        // decrement task_count
        task_count--;

        pthread_mutex_unlock(&mutex_queue);

        // execute the task
        execute_task(task);
    }
}

/**
 * @brief Create a stream server that listens on a TCP socket.
 * 
 * @return - file descriptor for socket of the server starting
 *           the server is successful.
 *              else returns -1.
 */
int
start_server(void)
{
    int server_socket;
    struct sockaddr_in server_addr;

    // create a stream socket of type AF_INET
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket < 0)
    {
        perror("Failed to create server socket\n");
        return EXIT_FAILURE;
    }
        
    // initialize the server_addr struct for IPv4 connection on any port
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(SERVER_PORT);

    // bind the socket to the port (server_addr)
    int ret = bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr));
    if (ret < 0)
    {
        perror("Socket failed to bind to TCP port\n");
        return EXIT_FAILURE;
    }

    socklen_t len = sizeof(server_addr);
    getsockname(server_socket, (struct sockaddr*)&server_addr, &len);
    printf("Server listening on port %d\n", ntohs(server_addr.sin_port));
    
    // start listening on the port
    ret = listen(server_socket, SERVER_BACKLOG);
    if (ret < 0) 
    {
        perror("Failed to listen on port\n");
        return EXIT_FAILURE;
    }

    return server_socket;
}

/**
 * @brief Execute a task on the task queue.
 * 
 * @param task - struct with information about the task.
 */
void
execute_task(SysinfoTask task)
{
    char method[64], path[256];
    char* request_buf = (char*)malloc(BUFSIZE);

    int ret = read_request(task.client_fd, request_buf);
    if (ret < 0)
    {
        perror("Could not read request\n");
    }

    parse_request(request_buf, method, path);
    handle_request(task.client_fd, method, path);
}

/**
 * @brief Add a task to the task queue
 * 
 * @param task - the SysinfoTask to add to the queue
 */
void
submit_task(SysinfoTask task)
{
    pthread_mutex_lock(&mutex_queue);
    task_queue[task_count] = task;
    task_count++;
    pthread_mutex_unlock(&mutex_queue);
    pthread_cond_signal(&cond_queue);
}
