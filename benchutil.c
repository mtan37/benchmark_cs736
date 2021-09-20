#include "benchutil.h"

unsigned long S_TO_NS = 1000000000;
uint64_t calculate_tick(unsigned a, unsigned b) {
    uint64_t tick;
    tick = (((uint64_t)a) | (((uint64_t)b) << 32));// calculating tick value
}

double tick_to_ns(uint64_t tick2, uint64_t tick1) {
    return (double)(tick2 - tick1)*S_TO_NS/3200000000;
}

