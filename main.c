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

#define BUFFER_SIZE 4096

int read_request(int client_socket, char *request_buffer);
void parse_request(char *request_buffer, char *method, char *path);
void handle_request(int client_socket, const char *method, const char *path);

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
            printf("accept failed\n");
        }

        char* request_buf = (char*) malloc(256);
        int read_ret = read_request(client_fd, request_buf);
        if (read_ret < 0)
        {
            printf("Couldn't read that\n");
            close(client_fd);
            continue;
        }
        parse_request(request_buf, method, path);
        handle_request(client_fd, method, path);

        printf("accepted client, got %s\n", request_buf);

        close(client_fd);
    }

    wait_on_worker_pool(worker_pool);

    return EXIT_SUCCESS;
}

int read_request(int client_socket, char *request_buffer)
{
    int bytes_received = recv(client_socket, request_buffer, BUFFER_SIZE - 1, 0);
    if (bytes_received < 0) {
        perror("recv failed");
        return -1;
    }
    request_buffer[bytes_received] = '\0'; // Null-terminate the string
    return bytes_received;
}

void parse_request(char *request_buffer, char *method, char *path)
{
    sscanf(request_buffer, "%s %s", method, path);
}

void handle_request(int client_socket, const char *method, const char *path)
{
    char response[BUFFER_SIZE];

    if (strcmp(method, "GET") == 0 && strcmp(path, "/cpu") == 0) {
        sprintf(response, 
            "HTTP/1.1 200 OK\r\n"
            "Content-Type: application/json\r\n"
            "Content-Length: 20\r\n"
            "\r\n"
            "{\"data\": \"cpu\"}");
    } else if (strcmp(method, "GET") == 0 && strcmp(path, "/memory") == 0) {
        sprintf(response, 
            "HTTP/1.1 200 OK\r\n"
            "Content-Type: application/json\r\n"
            "Content-Length: 20\r\n"
            "\r\n"
            "{\"data\": \"memory\"}");
    } else if (strcmp(method, "GET") == 0 && strcmp(path, "/disk") == 0) {
        sprintf(response, 
            "HTTP/1.1 200 OK\r\n"
            "Content-Type: application/json\r\n"
            "Content-Length: 20\r\n"
            "\r\n"
            "{\"data\": \"disk\"}");
    } else {
        sprintf(response, 
            "HTTP/1.1 404 Not Found\r\n"
            "Content-Length: 13\r\n"
            "Content-Type: text/plain\r\n"
            "\r\n"
            "404 Not Found");
    }

    send(client_socket, response, strlen(response), 0);
}