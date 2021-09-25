#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include "benchutil.h"

int make_connection(char *server_host) {
    int socket_fd;
    struct sockaddr_in server;
    const long SERVER_PORT = 23577;

    if (-1 == (socket_fd=socket(AF_INET, SOCK_STREAM, 0))){
        fprintf(stderr, "server socket creation failed\n");
        return -1;
    }
 
    // bind socket to a port 23576 
    server.sin_family = AF_INET;
    server.sin_port=htons(SERVER_PORT);
    server.sin_addr.s_addr = inet_addr(server_host);
    
    if (0 > connect(socket_fd, (struct sockaddr *)&server, sizeof(server))){
        fprintf(stderr, "To server %s connection failed\n", server_host); 
        return -1;
    }

    printf("DEBUG: server and client connected\n");

    return socket_fd;
}

int measure_latency(char *server_host, size_t m_size, int socket_fd){
    // START DATA RECEIVE
    
    char child_buf[m_size];
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
    const int TEST_COUNT = 50;
    char *host = argv[1];
    int m_sizes[10] = {4, 16, 64, 256, 1024, 4 * 1024, 16 * 1024, 64 * 1024, 256 * 1024, 512 * 1024};
    double results[10];

    //make connection
    int socket_fd = make_connection(host);    

    for (int i = 0; i < sizeof(m_sizes)/sizeof(int); i++) {
        // loop through different packet sizes
        for (int j = 0; j < TEST_COUNT; j++){
            while (-1 == measure_latency(host, m_sizes[i], socket_fd)){} 
        }
    }
    close(socket_fd);
}
