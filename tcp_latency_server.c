#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include "benchutil.h"

int *make_connection() {
    int socket_fd, client_socket;
    struct sockaddr_in server, client;
    const long LOCAL_PORT = 23577;

    if (-1 == (socket_fd=socket(AF_INET, SOCK_STREAM, 0))){
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
    printf("DEBUG: server binding to port %ld\n", LOCAL_PORT);
    // listen on the port
    listen(socket_fd, 1);

    printf("DEBUG: server waiting for incoming connection...\n");

    if (0 > (client_socket = (accept(socket_fd, NULL, NULL)))){
        fprintf(stderr, "accept client connection failed\n"); 
        exit(1);
    }
    printf("DEBUG: server and client connected\n");

    static int fd[2];
    fd[0] = socket_fd;
    fd[1] = client_socket; 
    return fd;
}

double measure_latency(size_t m_size, int client_socket){
    // START MEASUREMENT
    unsigned a, b, c, d;
    uint64_t tick1, tick2;
   
    size_t byte_written;
    size_t byte_read;
    size_t byte_left;
    char server_buf[m_size];
    memset(server_buf, '0', sizeof(server_buf));
    
    //rest for a second...
    sleep(1);  

    asm volatile("rdtsc" : "=a" (a), "=d" (b)); //assembly code running the instruction rdtsc
    /****/
    byte_written = write(client_socket, server_buf, m_size);
    if (-1 == byte_written || m_size != byte_written) {
        //printf("Parent write error. m_size %ld, byte write %ld, retry\n",
        //    m_size, byte_written);
        return -1;
    }
    
    byte_left = m_size;
    while( byte_left > 0 &&
        -1 != (byte_read = read(client_socket, server_buf, byte_left))){
        byte_left = byte_left - byte_read; 
    }
    /****/
    asm volatile("rdtsc" : "=a" (c), "=d" (d)); //assembly code running the instruction rdtsc

    // do computation for time elapsed...
    tick1 = calculate_tick(a,b);
    tick2 = calculate_tick(c,d);
    double diff = tick_to_ns(tick2, tick1);
    return diff;
      
}

int main(int argc , char *argv[]) {
    const int TEST_COUNT = 50;
    char *host = argv[1];
    int m_sizes[10] = {4, 16, 64, 256, 1024, 4 * 1024, 16 * 1024, 64 * 1024, 256 * 1024, 512 * 1024};
    double results[10];
    
    //make connection
    int *fd = make_connection();
    int socket_fd = fd[0];
    int client_socket = fd[1];

    for (int i = 0; i < sizeof(m_sizes)/sizeof(int); i++) {
        // loop through different packet sizes
        double result = -1;
        double diff;
        for (int j = 0; j < TEST_COUNT; j++){
            while (-1 == (diff = measure_latency(m_sizes[i], client_socket))){}  
            if (-1 == result || diff < result) {
                result = diff;
            }
        }
        //printf("m size at i %d,add result, %ld\n", i, m_sizes[i]);
        results[i] = result/2;
    }
    
    // close connection 
    close(socket_fd);
    
    for (int i = 0; i < sizeof(results)/sizeof(double); i++) {
        printf("***** TEST latency for tcp %s, package size %d Byte: %f\n", host, m_sizes[i], results[i]);
    }
}
