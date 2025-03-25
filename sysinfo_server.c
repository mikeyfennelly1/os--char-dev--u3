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

#define SERVER_PORT 8080
#define SERVER_BACKLOG 1

typedef struct SysinfoTask {
    int info_type;
    int client_fd;
} SysinfoTask;

SysinfoTask task_queue[256];
int task_count = 0;

pthread_mutex_t mutex_queue;
pthread_mutex_t mutex_get_sysinfo;
pthread_cond_t cond_queue;

void* start_thread(void* args);
void submit_task(SysinfoTask task);
void execute_task(SysinfoTask* task);
int create_server_and_listen_on_port(void);

/**
 * @brief - Create a thread pool of size 'num_workers'.
 * 
 * @param num_workers - the number of worker threads in the thread pool.
 * @param cond_var - the condition variable to change when all threads have been created.
 * 
 * @return integer status code.
 */
int
create_worker_pool(int num_workers,
                   pthread_cond_t* cond_var)
{
    // create array of thread references with size THREAD_NUM
    pthread_t th[num_workers];
    // init mutex for protecting queue operations
    pthread_mutex_init(&mutex_queue, NULL);
    // init cond_var for use when task is added to queue
    pthread_cond_init(&cond_queue, NULL);

    pthread_mutex_init(&mutex_get_sysinfo, NULL);

    // create thread pool
    for (int i = 0; i < num_workers; i++)
    {
        if (pthread_create(&th[i], NULL, &start_thread, NULL) != 0)
        {
            perror("Failed to create thread\n");
        }
    }

    // block main thread until all threads have joined.
    for (int i = 0; i < num_workers; i++)
    {
        if (pthread_join(th[i], NULL) != 0)
        {
            perror("Failed to join the thread\n");
        }
    }

    // destroy mutex_queue and cond_queue
    pthread_mutex_destroy(&mutex_queue);
    pthread_cond_destroy(&cond_queue);
    pthread_mutex_destroy(&mutex_get_sysinfo);

    return EXIT_SUCCESS;
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
        SysinfoTask task;
        task = task_queue[0];
        bool found = false;

        // Lock the critical section where task is being taken from queue.
        pthread_mutex_lock(&mutex_queue);
        while (task_count == 0)
        {
            pthread_cond_wait(&cond_queue, &mutex_queue);
        }

        found = true;
        for (int i = 0; i < task_count - 1; i++)
        {
            task_queue[i] = task_queue[i + 1];
        }
        task_count--;

        pthread_mutex_unlock(&mutex_queue);
        execute_task(&task);
    }
}

/**
 * @brief Create a stream server that listens on a TCP socket.
 * 
 * 
 * @return - file descriptor for socket of the server starting
 *           the server is successful.
 *              else returns -1.
 */
int
create_server_and_listen_on_port(void)
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

    // start listening on the port
    ret = listen(server_socket, SERVER_BACKLOG);
    if (ret < 0) 
    {
        perror("Failed to listen on port\n");
        return EXIT_FAILURE;
    }

    return server_socket;
}

void
execute_task(SysinfoTask* task)
{
}

/**
 * @brief Add a task to the task queue
 * 
 * @param task - the SysinfoTask to be added to the task queue
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
