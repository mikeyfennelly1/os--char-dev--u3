#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <pthread.h>

int main(void)
{
    // create array of thread references with size THREAD_NUM
    pthread_t th[THREAD_NUM];
    // init mutex for protecting queue operations
    pthread_mutex_init(&mutex_queue, NULL);
    // init cond_var for use when task is added to queue
    pthread_cond_init(&cond_queue, NULL);

    pthread_mutex_init(&mutex_get_sysinfo, NULL);

    // create thread pool
    for (int i = 0; i < THREAD_NUM; i++)
    {
        if (pthread_create(&th[i], NULL, &start_thread, NULL) != 0)
        {
            perror("Failed to create thread\n");
        }
    }

    int server_sock = create_server_and_listen_on_port();
    if (server_sock < 0)
    {
        perror("Failed to start server\n");
        return EXIT_FAILURE;
    }

    // block main thread until all threads have joined.
    for (int i = 0; i < THREAD_NUM; i++)
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
