#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <sys/socket.h>
#include "benchutil.h"
#include<sys/wait.h>
#include <errno.h>
#include <string.h>

const size_t BUF_SIZE = 512 * 1024;//512K bytes

double socketpair_c(const size_t m_size, const size_t total_size){
    int sv[2];
    pid_t child_pid;
    unsigned a, b, c, d;
    uint64_t tick1, tick2;
    size_t byte_written, byte_left, next_sent;       
 
    if (0 != socketpair(AF_UNIX, SOCK_STREAM, 0, sv)) {
        fprintf(stderr, "Socketpair call failed\n");
        exit(0);
    }
    
    child_pid = fork();    
    if (-1 == child_pid) {
        fprintf(stderr, "fork call failed\n");
        exit(0);
    }

    if (0 == child_pid) {
        // child process block
        // use the second socket
        close(sv[0]);
        //child process
        char child_buf[m_size];
        while(-1 != read(sv[1], child_buf, m_size)){
        }
    
        exit(0);
    } else {
        // parent process block
        // use the first socket
        close(sv[1]);
        // parent process sends first
        char parent_buf[m_size];
        memset(parent_buf, '0', sizeof(parent_buf));

        /****/
        byte_left = total_size;
        next_sent = min(m_size, byte_left);
        asm volatile("rdtsc" : "=a" (a), "=d" (b)); //assembly code running the instruction rdtsc
        while (next_sent > 0 &&
            -1 != (byte_written = write(sv[0], parent_buf, next_sent))){
            byte_left = byte_left - byte_written;
            next_sent = min(m_size, byte_left);     
            //memset(parent_buf, '0', next_sent);
            //printf("Parent: byte write: %ld, byte left: %ld\n", byte_written, byte_left);
        }
        /****/
        asm volatile("rdtsc" : "=a" (c), "=d" (d)); //assembly code running the instruction rdtsc

        kill(child_pid, SIGKILL);        
        
        // do computation for time elapsed...
        tick1 = calculate_tick(a,b);
        tick2 = calculate_tick(c,d);
        double diff = tick_to_ns(tick2, tick1);
    }
}

int main(int argc, char*argv[]){
    const int TEST_COUNT = 20;
    const int DATA_SIZE = 100;// in mb
    int m_sizes[10] = {4, 16, 64, 256, 1024, 4 * 1024, 16 * 1024, 64 * 1024, 256 * 1024, 512 * 1024};
    double results[10];
    for (int i = 0; i < sizeof(m_sizes)/sizeof(int); i++) {
        //printf("m size at i %d, %ld\n", i, m_sizes[i]);
        // loop through different packet sizes
        double result = -1;
        double diff;
        for (int j = 0; j < TEST_COUNT; j++){
            diff = socketpair_c(m_sizes[i], DATA_SIZE * MB_TO_B);  
            if (-1 == result || diff < result) {
                result = diff;
            }
        }
        results[i] = ((double)DATA_SIZE)/((double)result/S_TO_NS);
    }
    
    for (int i = 0; i < sizeof(results)/sizeof(double); i++) {
        printf("***** TEST throughput for socket pair, package size %d Speed: %f MiB/s\n", m_sizes[i], results[i]);
    }
}
