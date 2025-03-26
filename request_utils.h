#ifndef REQUEST_UTILS_H
#define REQUEST_UTILS_H

/**
 * @brief Read a request from a client socket into a request buffer.
 * 
 * @param client_socket - the file descriptor for the client socket to read from.
 * @param request_buffer - buffer to write the contents from the read into.
 * 
 * @return number of bytes received from the client.
 */
int read_request(int client_socket, char *request_buffer);

/**
 * @brief Parse the request from the client.
 * 
 * Get the method and the path from the request. Write the method 
 * and path into their respective buffers, provided by caller.
 * 
 * @param method - buffer to write the parsed method into.
 * @param path - buffer to write the parse path into.
 */
void parse_request(char *request_buffer, char *method, char *path);

/**
 * @brief Write a response to the client socket, given method and path.
 * 
 * @param client_socket - file descriptor for the client socket to respond to.
 * @param method - the string for the HTTP method (provided by the client request).
 * @param path - the client's requested route.
 * 
 */
void handle_request(int client_socket, const char *method, const char *path);

#endif