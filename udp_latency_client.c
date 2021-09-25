#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include "benchutil.h"

int measure_latency(char *server_host, size_t m_size, int socket_fd, int SERVER_PORT){
    // START MEASUREMENT
    unsigned a, b, c, d;
    uint64_t tick1, tick2;   
    struct sockaddr_in server;
 
    char client_buf[m_size * 5];
    size_t byte_left, byte_read, byte_written;
 
    // server socket configuration 
    server.sin_family = AF_INET;
    server.sin_port=htons(SERVER_PORT);
    server.sin_addr.s_addr = inet_addr(server_host);
    socklen_t slen = sizeof(server);

    // take a rest...
    sleep(0.5);

    asm volatile("rdtsc" : "=a" (a), "=d" (b)); //assembly code running the instruction rdtsc
    /****/
    byte_written = sendto(socket_fd, client_buf, m_size, 0, (struct sockaddr *) &server, slen);
    //printf("DEBUG: client write packet size %ld to server\n", m_size);
    if (-1 == byte_written || m_size != byte_written) {
        printf("Client write error. m_size %ld, byte write %ld, retry\n",
            m_size, byte_written);
        return -1;
    }

    byte_left = m_size;
    while( byte_left > 0 &&
        -1 != (byte_read = recvfrom(socket_fd, client_buf, byte_left, 0, (struct sockaddr *)&server, &slen))){
        byte_left = byte_left - byte_read;
    }
    //printf("DEBUG: client received packet size %ld from server\n", m_size);

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
    int remote_port = strtol(argv[2], &ptr, 10);
    const int TEST_COUNT = strtol(argv[3], &ptr, 10);

    int m_sizes[7] = {4, 16, 64, 256, 1024, 4 * 1024, 16 * 1024};
    double results[7];

    //create socket endpoint
    int socket_fd;
    if (-1 == (socket_fd=socket(AF_INET, SOCK_DGRAM, 0))){
        fprintf(stderr, "client socket creation failed\n");
        return -1;
    }
    
    for (int i = 0; i < sizeof(m_sizes)/sizeof(int); i++) {
        // loop through different packet sizes
        double result = -1;
        double diff;
        for (int j = 0; j < TEST_COUNT; j++){
            if (-1 == (diff = measure_latency(host, m_sizes[i], socket_fd, remote_port))){
                printf("write for m_size %d failed\n", m_sizes[i]);
            } 
            if (-1 == result || diff < result) {
                result = diff;
            }
        }
        results[i] = result/2;
    }
    close(socket_fd);
    
    for (int i = 0; i < sizeof(results)/sizeof(double); i++) {
        printf("***** TEST latency for tcp %s, package size %d Byte: %f\n", host, m_sizes[i], results[i]);
    }

}

