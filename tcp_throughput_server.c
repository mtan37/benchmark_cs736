#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include "benchutil.h"

int *make_connection(const long LOCAL_PORT) {
    int socket_fd, client_socket;
    struct sockaddr_in server, client;

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

double measure_throughput(size_t m_size, int client_socket, size_t packet_count){
    // START MEASUREMENT
    unsigned a, b, c, d;
    uint64_t tick1, tick2;
   
    size_t byte_written;
    char server_buf[m_size];
    memset(server_buf, '0', sizeof(server_buf));
    
    //rest for a second...
    sleep(1);  

    asm volatile("rdtsc" : "=a" (a), "=d" (b)); //assembly code running the instruction rdtsc
    /****/
    
    while (packet_count > 0 &&
    -1 != (byte_written = write(client_socket, server_buf, m_size))){
        if (byte_written != m_size) {
            //write not successful, run abort
            printf("Parent write error. m_size %ld, byte write %ld, retry\n", m_size, byte_written);
            return -1;
        }
        packet_count = packet_count - 1;
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
    char *host = argv[1];
    char *ptr;
    int port = strtol(argv[2], &ptr, 10);
    const int TEST_COUNT = strtol(argv[3], &ptr, 10);
    int m_sizes[10] = {4, 16, 64, 256, 1024, 4 * 1024, 16 * 1024, 64 * 1024, 256 * 1024, 512 * 1024};
    double results[10];

    const int DATA_SIZE = 100 * MB_TO_B;//in B

    //make connection
    int *fd = make_connection(port);
    int socket_fd = fd[0];
    int client_socket = fd[1];
    for (int i = 0; i < sizeof(m_sizes)/sizeof(int); i++) {
        // loop through different packet sizes
        double result = -1;
        double diff;
        const int PACKET_COUNT = DATA_SIZE/m_sizes[i];
        for (int j = 0; j < TEST_COUNT; j++){
            while (-1 == (diff = measure_throughput(m_sizes[i], client_socket, PACKET_COUNT))){}  
            if (-1 == result || diff < result) {
                result = diff;
            }
        }
        results[i] = ((double)DATA_SIZE/MB_TO_B)/((double)result/S_TO_NS);
    } 
    // close connection 
    close(socket_fd);
   
    for (int i = 0; i < sizeof(results)/sizeof(double); i++) { 
        printf("***** TEST throughput for tcp %s, package size %d Speed %f MiB/s\n", host, m_sizes[i], results[i]);
    }
}
