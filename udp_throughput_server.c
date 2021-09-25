#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include "benchutil.h"

int *bind_port(const long LOCAL_PORT) {
    int socket_fd, client_socket;
    struct sockaddr_in server, client;

    if (-1 == (socket_fd=socket(AF_INET, SOCK_DGRAM, 0))){
        fprintf(stderr, "server socket creation failed\n");
        exit(1);
    }
 
    // bind socket to a port 23577 
    server.sin_family = AF_INET;
    server.sin_port=htons(LOCAL_PORT);
    server.sin_addr.s_addr = htonl(INADDR_ANY);
    
    if (-1 == bind(socket_fd, (struct sockaddr *)&server, sizeof(server))) {
        fprintf(stderr, "server bind failed\n"); 
        exit(1);
    }
    //printf("DEBUG: server binding to port %ld\n", LOCAL_PORT);
    static int fd[1];
    fd[0] = socket_fd;
    return fd;
}

double measure_throughput(size_t m_size, int server_socket){
    size_t byte_written, byte_read, byte_left;
    char server_buf[m_size * 5];
    struct sockaddr_in client_addr;
    socklen_t slen = sizeof(client_addr);
    
    while(1 != (byte_read = recvfrom(server_socket, server_buf, byte_left, 0, (struct sockaddr *) &client_addr, &slen))){
        byte_left = byte_left - byte_read; 
    }
     
    return 0; 
}

int main(int argc , char *argv[]) {
    char *host = argv[1];
    char *ptr;
    int port = strtol(argv[2], &ptr, 10);
    int m_sizes[7] = {4, 16, 64, 256, 1024, 4 * 1024, 16 * 1024};

    //bind to port
    int *fd = bind_port(port);
    int socket_fd = fd[0];
    measure_throughput(m_sizes[6], socket_fd);  
    // close connection 
    close(socket_fd);
}
