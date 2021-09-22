#ifndef BENCHUTIL
#define BENCHUTIL
#include <inttypes.h>
#include <stddef.h>
extern unsigned long S_TO_NS;
extern unsigned long MB_TO_B;
uint64_t calculate_tick(unsigned a, unsigned b);
double tick_to_ns(uint64_t tick2, uint64_t tick1);
size_t min(size_t x, size_t y);
#endif
