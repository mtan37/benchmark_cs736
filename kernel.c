#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include "benchutil.h"
#include <sys/stat.h>

void getpid_test(){
    unsigned long s_to_ns = 1000000000;
    uint64_t tick1, tick2; //unsigned 64 bit quantity
    unsigned a, b, c, d;
    double result = -1;
    const int test_count = 100000;    
    for (int i = 0; i < test_count; i++){
        asm volatile("rdtsc" : "=a" (a), "=d" (b)); //assembly code running the instruction rdtsc
        /*sys call go here⌄⌄⌄⌄⌄⌄*/
        getpid();
        /*sys call go here^^^^^^*/
        asm volatile("rdtsc" : "=a" (c), "=d" (d)); //assembly code running the instruction rdtsc
        tick1 = calculate_tick(a,b);
        tick2 = calculate_tick(c,d);
        double diff = (double)(tick2 - tick1)*s_to_ns/3200000000;
        //printf("#%d getpid() elapsed time(double) %f\n",i+1, diff);
    
        if (result == -1 || diff < result){
            result = diff;
        }
    }
    printf("****** getpid() elapsed time result %f\n", result);
}


void getuid_test(){
    unsigned long s_to_ns = 1000000000;
    uint64_t tick1, tick2; //unsigned 64 bit quantity
    unsigned a, b, c, d;
    double result = -1;
    const int test_count = 100000;    
    for (int i = 0; i < test_count; i++){
        asm volatile("rdtsc" : "=a" (a), "=d" (b)); //assembly code running the instruction rdtsc
        /*sys call go here⌄⌄⌄⌄⌄⌄*/
        getuid();
        /*sys call go here^^^^^^*/
        asm volatile("rdtsc" : "=a" (c), "=d" (d)); //assembly code running the instruction rdtsc
        tick1 = calculate_tick(a,b);
        tick2 = calculate_tick(c,d);
        double diff = (double)(tick2 - tick1)*s_to_ns/3200000000;
        //printf("#%d getpid() elapsed time(double) %f\n",i+1, diff);
    
        if (result == -1 || diff < result){
            result = diff;
        }
    }
    printf("****** getuid() elapsed time result %f\n", result);
}

void stat_test(){
    unsigned long s_to_ns = 1000000000;
    uint64_t tick1, tick2; //unsigned 64 bit quantity
    unsigned a, b, c, d;
    double result = -1;
    const int test_count = 100000; 

    struct stat tmp;

    for (int i = 0; i < test_count; i++){
        asm volatile("rdtsc" : "=a" (a), "=d" (b)); //assembly code running the instruction rdtsc
        /*sys call go here⌄⌄⌄⌄⌄⌄*/
        stat("/proc/cpuinfo", &tmp);
        /*sys call go here^^^^^^*/
        asm volatile("rdtsc" : "=a" (c), "=d" (d)); //assembly code running the instruction rdtsc
        tick1 = calculate_tick(a,b);
        tick2 = calculate_tick(c,d);
        double diff = (double)(tick2 - tick1)*s_to_ns/3200000000;
        //printf("#%d getpid() elapsed time(double) %f\n",i+1, diff);
    
        if (result == -1 || diff < result){
            result = diff;
        }
    }
    printf("****** stat() elapsed time result %f\n", result);
}

int main(int argc, char*argv[]){
    getpid_test();
    getuid_test();
    stat_test();
}

