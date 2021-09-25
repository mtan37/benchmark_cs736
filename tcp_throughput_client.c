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
    server.sin_addr.s_addr = inet_addr(server_host);
    //server.sin_addr.s_addr = htonl(INADDR_ANY);
    
    if (0 > connect(socket_fd, (struct sockaddr *)&server, sizeof(server))){
        fprintf(stderr, "To server %s connection failed\n", server_host); 
        return -1;
    }

    printf("DEBUG: server and client connected\n");

    return socket_fd;
}

int main(int argc , char *argv[]) {
    char *host = argv[1];
    
    char *ptr;
    int remote_port = strtol(argv[2], &ptr, 10);
    const int TEST_COUNT = strtol(argv[3], &ptr, 10);

    //make connection
    int socket_fd = make_connection(host, remote_port);    
    
    char client_buf[512 * 1024 * 2];
    while(-1 != read(socket_fd, client_buf, sizeof(client_buf))){}
    close(socket_fd);
}
