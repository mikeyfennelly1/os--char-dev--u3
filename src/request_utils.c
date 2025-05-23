/**
 * request_utils.c
 * 
 * Utilities for handling GET requests from clients.
 * 
 * @author Mikey Fennelly
 */

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include "sysinfo_device.h"
#include "sysinfo_server.h"

#define BUFFER_SIZE 4096

/**
 * @brief Read a request from a client socket into a request buffer.
 * 
 * @param client_socket - the file descriptor for the client socket to read from.
 * @param request_buffer - buffer to write the contents from the read into.
 * 
 * @return number of bytes received from the client.
 */
int
read_request(int client_socket,
             char *request_buffer)
{
    int bytes_received = recv(client_socket, request_buffer, BUFFER_SIZE - 1, 0);
    if (bytes_received < 0) {
        perror("recv failed");
        return -1;
    }
    request_buffer[bytes_received] = '\0'; // Null-terminate the string
    return bytes_received;
}

/**
 * @brief Parse the request from the client.
 * 
 * Get the method and the path from the request. Write the method 
 * and path into their respective buffers, provided by caller.
 * 
 * @param method - buffer to write the parsed method into.
 * @param path - buffer to write the parse path into.
 */
void
parse_request(char *request_buffer,
              char *method,
              char *path)
{
    sscanf(request_buffer, "%s %s", method, path);
}

/**
 * @brief Write a response to the client socket, given method and path.
 * 
 * @param client_socket - file descriptor for the client socket to respond to.
 * @param method - the string for the HTTP method (provided by the client request).
 * @param path - the client's requested route.
 * 
 */
void handle_request(int client_socket, const char *method, const char *path) {
    char response[BUFFER_SIZE];
    char sysinfo[BUFFER_SIZE];
    int content_length;
    
    printf("path: %s\n", path);
    printf("method: %s\n", method);

    pthread_mutex_lock(&mutex_get_sysinfo);
    if (strcmp(method, "GET") == 0) {
        if (strcmp(path, "/cpu") == 0) {
            snprintf(sysinfo, BUFFER_SIZE, "%s", get_sysinfo(CPU));
        } else if (strcmp(path, "/memory") == 0) {
            snprintf(sysinfo, BUFFER_SIZE, "%s", get_sysinfo(MEMORY));
        } else if (strcmp(path, "/disk") == 0) {
            snprintf(sysinfo, BUFFER_SIZE, "%s", get_sysinfo(DISK));
        } else {
            pthread_mutex_unlock(&mutex_get_sysinfo);
            snprintf(response, BUFFER_SIZE, 
                "HTTP/1.1 404 Not Found\r\n"
                "Content-Length: 13\r\n"
                "Content-Type: text/plain\r\n"
                "\r\n"
                "404 Not Found");
            send(client_socket, response, strlen(response), 0);
            close(client_socket);
            return;
        }

        pthread_mutex_unlock(&mutex_get_sysinfo);
        
        content_length = strlen(sysinfo);
        int header_size = snprintf(response, BUFFER_SIZE, 
            "HTTP/1.1 200 OK\r\n"
            "Content-Type: application/json\r\n"
            "Content-Length: %d\r\n"
            "\r\n",
            content_length);

        if (header_size < BUFFER_SIZE) {
            strncat(response, sysinfo, BUFFER_SIZE - header_size - 1);
        }
    } else {
        snprintf(response, BUFFER_SIZE,
            "HTTP/1.1 405 Method Not Allowed\r\n"
            "Content-Length: 23\r\n"
            "Content-Type: text/plain\r\n"
            "\r\n"
            "405 Method Not Allowed");
        pthread_mutex_unlock(&mutex_get_sysinfo);
    }

    printf("Response:\n\n %s\n", response);
    send(client_socket, response, strlen(response), 0);
    close(client_socket);
}