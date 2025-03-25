#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <pthread.h>

#include "sysinfo_server.h"

int main(void)
{


    int server_sock = create_server_and_listen_on_port();
    if (server_sock < 0)
    {
        perror("Failed to start server\n");
        return EXIT_FAILURE;
    }

    ThreadPool* thread_pool =  create_worker_pool(10);

    wait_on_thread_pool(thread_pool);
    return EXIT_SUCCESS;
}
