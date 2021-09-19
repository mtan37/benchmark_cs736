#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>

uint64_t calculate_tick(unsigned a, unsigned b) {
    uint64_t tick;
    tick = (((uint64_t)a) | (((uint64_t)b) << 32));// calculating tick value
}

void getpid_test(){
    unsigned long S_TO_NS = 1000000000;
    uint64_t tick1, tick2; //unsigned 64 bit quantity
    unsigned a, b, c, d;
    double result = -1;
    const int TEST_COUNT = 100000;    
    for (int i = 0; i < TEST_COUNT; i++){
        asm volatile("rdtsc" : "=a" (a), "=d" (b)); //assembly code running the instruction rdtsc
        getpid();
        asm volatile("rdtsc" : "=a" (c), "=d" (d)); //assembly code running the instruction rdtsc
        tick1 = calculate_tick(a,b);
        tick2 = calculate_tick(c,d);
        double diff = (double)(tick2 - tick1)*S_TO_NS/3200000000;
        printf("#%d getpid() elapsed time(double) %f\n",i+1, diff);
    
        if (result == -1 || diff < result){
            result = diff;
        }
    }
    printf("****** getpid() elapsed time result %f\n", result);
}

int main(int argc, char*argv[]){
    getpid_test();
}

