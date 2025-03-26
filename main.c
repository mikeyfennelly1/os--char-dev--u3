/**
 * main.c
 * 
 * Entrypoint to sysinfo server
 * 
 * @author Mikey Fennelly
 */

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
    int server_sock = start_server();
    if (server_sock < 0)
    {
        perror("Failed to start server\n");
        return EXIT_FAILURE;
    }

    WorkerPool* worker_pool = create_worker_pool(10);
    
    struct sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);

    char method[16], path[256];

    while(1)
    {
        int client_fd = accept(server_sock, (struct sockaddr*)&client_addr, &client_len);
        if (client_fd < 0)
        {
            printf("Accept failure");
            continue;
        }
        SysinfoTask this_task = {client_fd};
        submit_task(this_task);
        printf("Task submitted\n");
    }

    wait_on_worker_pool(worker_pool);

    return EXIT_SUCCESS;
}