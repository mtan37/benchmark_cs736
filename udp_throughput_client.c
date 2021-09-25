#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include "benchutil.h"

double measure_throughput(char *server_host, size_t m_size, int socket_fd, int SERVER_PORT, size_t packet_count){
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
    //printf("DEBUG: packet count %ld for size %ld\n", packet_count, m_size);
    asm volatile("rdtsc" : "=a" (a), "=d" (b)); //assembly code running the instruction rdtsc
    /****/
    
    while(packet_count > 0 &&
        -1 != (byte_written = sendto(socket_fd, client_buf, m_size, 0, (struct sockaddr *) &server, slen))){
        if (m_size != byte_written) {
            printf("Client write error. m_size %ld, byte write %ld\n",
                m_size, byte_written);
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
    int remote_port = strtol(argv[2], &ptr, 10);
    const int TEST_COUNT = strtol(argv[3], &ptr, 10);

    int m_sizes[7] = {4, 16, 64, 256, 1024, 4 * 1024, 16 * 1024};
    double results[7];
    const size_t DATA_SIZE = 100 * MB_TO_B;// in B

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
        const int PACKET_COUNT = DATA_SIZE/m_sizes[i];
        for (int j = 0; j < TEST_COUNT; j++){
            if (-1 == (diff = measure_throughput(host, m_sizes[i], socket_fd, remote_port, PACKET_COUNT))){
                printf("write for m_size %d failed\n", m_sizes[i]);
            }
            if (-1 == result || diff < result) {
                result = diff;
            }
        }
        results[i] = ((double)DATA_SIZE/MB_TO_B)/((double)result/S_TO_NS);
        printf("***** TEST throughput for udp %s, package size %d Speed: %f MiB/s\n", host, m_sizes[i], results[i]);
    }
    close(socket_fd);

}

