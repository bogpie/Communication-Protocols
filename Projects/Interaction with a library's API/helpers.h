#ifndef _HELPERS_
#define _HELPERS_

#define HEADER_TERMINATOR "\r\n\r\n"
#define HEADER_TERMINATOR_SIZE (sizeof(HEADER_TERMINATOR) - 1)
#define CONTENT_LENGTH "Content-Length: "
#define CONTENT_LENGTH_SIZE (sizeof(CONTENT_LENGTH) - 1)

#define BUFLEN 4096
#define LINELEN 1000

#define PORT 8080
#define FLAG 0

#include <stdlib.h> // exit, atoi, malloc, free
#include <stdio.h>
#include <unistd.h>     // operate with file descriptors
#include <string.h>     // memory operations
#include <sys/socket.h> // server connection
#include <netinet/in.h> // address structures
#include <netdb.h>      // gethostbyname, hostent structure
#include <arpa/inet.h>
#include "buffer.h"
#include <iostream>
#include <string>
#include <sstream> // string streams

#include "nlohmann/json.hpp"

// Prints error
void error(const char *msg);

// Creates message using \r and \n separators
void computeMessage(std::string &message, std::string line);

// Creates a socket for a connection to the server host, using given port.
int open_connection(char *host_ip, int portno, int ip_type, int socket_type, int flag);

// Send message to server
void send_to_server(int sockfd, char *message);

// Receive message from server
char *receive_from_server(int sockfd);

#endif
