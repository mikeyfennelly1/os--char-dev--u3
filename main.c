#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <pthread.h>

#define PORT 8080
#define BUFFER_SIZE 1024

void* handle_client(void *client_sock_fd);

int main(void)
{
    int server_sock_fd, client_sock_fd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_len = sizeof(client_addr);

    server_sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_sock_fd < 0)
    {
        perror("Socket creation failed\n");
        return EXIT_FAILURE;
    }
   
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    int ret = bind(server_sock_fd, (struct sockaddr*)&server_addr, sizeof(server_addr));
    if (ret < 0)
    {
        perror("bind failed\n");
        close(server_sock_fd);
        return EXIT_FAILURE;
    }

    ret = listen(server_sock_fd, 5);
    if (ret < 0)
    {
        perror("listen fail\n");
        close(server_sock_fd);
        return EXIT_FAILURE;
    }

    printf("Server listening on port: %d\n", PORT);

    while (1)
    {
        client_sock_fd = accept(server_sock_fd, (struct sockaddr*)&client_addr, &addr_len);
        if (client_sock_fd < 0)
        {
            perror("Accept failed\n");
            continue;
        }

        pthread_t handling_thread;

        pthread_create(&handling_thread, NULL, handle_client, NULL);
    }
}

void* handle_client(void *client_sock_fd)
{

}