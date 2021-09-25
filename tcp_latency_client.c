#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include "benchutil.h"

int make_connection(char *server_host, const long SERVER_PORT) {
    int socket_fd;
    struct sockaddr_in server;

    if (-1 == (socket_fd=socket(AF_INET, SOCK_STREAM, 0))){
        fprintf(stderr, "server socket creation failed\n");
        return -1;
    }
 
    // bind socket to a port 23576 
    server.sin_family = AF_INET;
    server.sin_port=htons(SERVER_PORT);
    //server.sin_addr.s_addr = inet_addr(server_host);
    server.sin_addr.s_addr = htonl(INADDR_ANY);
    
    if (0 > connect(socket_fd, (struct sockaddr *)&server, sizeof(server))){
        fprintf(stderr, "To server %s connection failed\n", server_host); 
        return -1;
    }

    printf("DEBUG: server and client connected\n");

    return socket_fd;
}

int measure_latency(char *server_host, size_t m_size, int socket_fd){
    // START DATA RECEIVE
    
    char child_buf[m_size * 5];
    size_t byte_left, byte_read, byte_written;

    byte_left = m_size;
    while( byte_left > 0 &&
        -1 != (byte_read = read(socket_fd, child_buf, byte_left))){
        byte_left = byte_left - byte_read;
    }

    byte_left = m_size;
    while (byte_left > 0 &&
        -1 != (byte_written = write(socket_fd, child_buf, byte_left))){
        byte_left = byte_left - byte_written;
    }
    
    return 0; 
}

int main(int argc , char *argv[]) {
    char *host = argv[1];
    
    char *ptr;
    int m_size = strtol(argv[2], &ptr, 10);
    int remote_port = strtol(argv[3], &ptr, 10);
    const int TEST_COUNT = strtol(argv[4], &ptr, 10);

    //make connection
    int socket_fd = make_connection(host, remote_port);    

    for (int j = 0; j < TEST_COUNT; j++){
        while (-1 == measure_latency(host, m_size, socket_fd)){} 
    }
    close(socket_fd);
}
