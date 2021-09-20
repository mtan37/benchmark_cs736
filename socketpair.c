#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <sys/socket.h>
#include "benchutil.h"

const int BUF_SIZE = 512000;//512K bytes

void send_c(int s_fd, const int m_size, char *buf){
    if (m_size > sizeof(buf)){
        fprintf(stderr, "Wrong argument passed in for send()_c. buf size: %ld, m_size, %d\n", sizeof(buf), m_size);
        exit(0);
    }
    write(s_fd, buf, m_size); 
}

void receive_c(int s_fd, const int m_size, char *buf) {
    if (m_size > sizeof(buf)){
        fprintf(stderr, "Wrong argument passed in for receive_c(). buf size: %ld, m_size, %d\n", sizeof(buf), m_size);
        exit(0);
    }
    read(s_fd, buf, m_size); 
}

void socketpair_c(const int m_size){
    int sv[2];
    pid_t child_pid;
    unsigned a, b, c, d;
    uint64_t tick1, tick2;
    
    if (0 != socketpair(AF_UNIX, SOCK_STREAM, 0, sv)) {
        fprintf(stderr, "Socketpair call failed\n");
        return;
    }
    
    child_pid = fork();    
    if (-1 == child_pid) {
        fprintf(stderr, "fork call failed\n");
        return;
    }

    if (0 == child_pid) {
        // child process block
        // use the second socket
        close(sv[0]);
        //child process
        char child_buf[BUF_SIZE];
        receive_c(sv[1], m_size, child_buf);
        send_c(sv[1], m_size, child_buf);
        return;
    } else {
        // parent process block
        // use the first socket
        close(sv[1]);
        // parent process sends first
        char parent_buf[BUF_SIZE];
        
        asm volatile("rdtsc" : "=a" (a), "=d" (b)); //assembly code running the instruction rdtsc
        
        send_c(sv[1], m_size, parent_buf);
        receive_c(sv[1], m_size, parent_buf);
        
        asm volatile("rdtsc" : "=a" (c), "=d" (d)); //assembly code running the instruction rdtsc
        
    }
        // do computation for time elapsed...
        tick1 = calculate_tick(a,b);
        tick2 = calculate_tick(c,d);
        double diff = tick_to_ns(tick2, tick1);
        printf("#%d socketpair elapsed time(double) %f\n",1, diff); 
}

int main(int argc, char*argv[]){
    int m_sizes[10] = {4, 16, 64, 256, 1000, 4000, 16000, 64000, 256000, 512000};
    socketpair_c(4);  
}
