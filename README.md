# benchmark_cs736

## Measurement #1: Precision
measured rdstsc and clock, with rdtsc being more precise
## Measurement #2: Kernel
```
call 1 - getpid
call 2 - getuid
call 3 - stat, for file /proc/cpuinfo
```
## Measurement #3: Communication
