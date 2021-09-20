#ifndef BENCHUTIL
#define BENCHUTIL
#include <inttypes.h>
extern unsigned long S_TO_NS;
uint64_t calculate_tick(unsigned a, unsigned b);
double tick_to_ns(uint64_t tick2, uint64_t tick1);
#endif
