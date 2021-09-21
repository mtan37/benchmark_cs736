#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <sys/socket.h>
#include "benchutil.h"
#include<sys/wait.h>
#include <errno.h>
#include <string.h>

const size_t BUF_SIZE = 912000;//512K bytes

double socketpair_c(const size_t m_size){
    int sv[2];
    pid_t child_pid;
    unsigned a, b, c, d;
    uint64_t tick1, tick2;
   
    size_t byte_written;
    size_t byte_read;
    size_t byte_left;
    
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
        byte_left = m_size;
        while( byte_left > 0 &&
            -1 != (byte_read = read(sv[1], child_buf, byte_left))){
            byte_left = byte_left - byte_read; 
//`            printf("child, byte read: %ld, byte left: %ld\n", byte_read, byte_left);
        }

        byte_left = m_size;
        while (byte_left > 0 &&
            -1 != (byte_written = write(sv[1], child_buf, byte_left))){
            byte_left = byte_left - byte_written;      
            //printf("Child: byte write: %ld, byte left: %ld\n", byte_written, byte_left);
        }
        //printf("child byte read%ld, error: %d\n", byte_read, errno);
    
        exit(0);
    } else {
        // parent process block
        // use the first socket
        close(sv[1]);
        // parent process sends first
        char parent_buf[m_size];
        memset(parent_buf, '0', sizeof(parent_buf));

        asm volatile("rdtsc" : "=a" (a), "=d" (b)); //assembly code running the instruction rdtsc
        /****/
        byte_left = m_size;
        while (byte_left > 0 &&
            -1 != (byte_written = write(sv[0], parent_buf, byte_left))){
            byte_left = byte_left - byte_written;      
            //printf("Parent: byte write: %ld, byte left: %ld\n", byte_written, byte_left);
        }
        
        byte_left = m_size;
        while( byte_left > 0 &&
            -1 != (byte_read = read(sv[0], parent_buf, byte_left))){
            byte_left = byte_left - byte_read; 
            //printf("Parent: byte read: %ld, byte left: %ld\n", byte_read, byte_left);
        }
        //printf("parent byte read%ld, error: %d\n", byte_read, errno);
        /****/
        asm volatile("rdtsc" : "=a" (c), "=d" (d)); //assembly code running the instruction rdtsc
        
        wait(NULL);    
    
        // do computation for time elapsed...
        tick1 = calculate_tick(a,b);
        tick2 = calculate_tick(c,d);
        double diff = tick_to_ns(tick2, tick1);
    }
}

int main(int argc, char*argv[]){
    const int TEST_COUNT = 100;
    int m_sizes[10] = {4, 16, 64, 256, 1000, 4000, 16000, 64000, 256000, 512000};
    double results[10];
    for (int i = 0; i < sizeof(m_sizes)/sizeof(int); i++) {
        //printf("m size at i %d, %ld\n", i, m_sizes[i]);
        // loop through different packet sizes
        double result = -1;
        double diff;
        for (int j = 0; j < TEST_COUNT; j++){
            diff = socketpair_c(m_sizes[i]);  
            //printf("#%d socketpair latency (double) %f\n",j+1, diff); 
            if (-1 == result || diff < result) {
                result = diff;
            }
        }
        //printf("m size at i %d,add result, %ld\n", i, m_sizes[i]);
        results[i] = result/2;
    }
    
    for (int i = 0; i < sizeof(results)/sizeof(double); i++) {
        printf("***** TEST latency for socket pair, package size %d Byte: %f\n", m_sizes[i], results[i]);
    }
}
