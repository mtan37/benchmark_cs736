#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>
#include <sys/time.h>
#include <time.h>

/*
 * Source: https://tuxthink.blogspot.com/2010/05/using-rdtsc-with-gcc.html
 * Return the CPU clock ticks
 * DO NOT CALL THIS METHOD, used as a reference
 */
void rdtsc_tick() {

    uint64_t tick1, tick2; //unsigned 64 bit quantity

    unsigned c,d;

    asm volatile("rdtsc" : "=a" (c), "=d" (d)); //assembly code running the instruction rdtsc

    tick1 = (((uint64_t)c) | (((uint64_t)d) << 32)); // calculating the tick value.

    printf("time %lu",tick1);
}

/*
 * Reference: https://www.gnu.org/software/libc/manual/html_node/CPU-Time.html
 */
void precision_test_2(){
    unsigned long S_TO_NS = 1000000000;
    double result;
    for (int i = 0; i < 10; i++) {
        clock_t start, end;
        double cpu_time_used;
        start = clock();
        int x = 1;
        end = clock();
        cpu_time_used = (double)((end - start) * S_TO_NS / CLOCKS_PER_SEC);
        printf("TEST2: %d: clock(3) elapsed time %f\n",i, cpu_time_used);
    
        if (cpu_time_used != 0 && cpu_time_used < result) {
            result = cpu_time_used
        }
    }

    printf("******TEST2 RESULT: clock(3) elapsed time %f\n", result);
}

void precision_test_1(){
    unsigned long S_TO_NS = 1000000000;

    for (int i = 0; i < 10; i++) {
        uint64_t tick1, tick2; //unsigned 64 bit quantity
        unsigned a, b, c, d;
        int x,y,z;
        asm volatile("rdtsc" : "=a" (a), "=d" (b)); //assembly code running the instruction rdts
        x = 1;
        y = 2;
        z = 3;
        asm volatile("rdtsc" : "=a" (c), "=d" (d)); //assembly code running the instruction rdtsc
        tick1 = (((uint64_t)a) | (((uint64_t)b) << 32)); // calculating the tick value.
        tick2 = (((uint64_t)c) | (((uint64_t)d) << 32)); // calculating the tick value.
        printf("TEST1: %d , rdtsc elapsed time %lu\n",i, (unsigned)(tick2 - tick1)*S_TO_NS/3200000000);
    }
}

int main(int argc, char*argv[]){
    precision_test_1();
    precision_test_2();
}

//clock_getres
//time(7)
