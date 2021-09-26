# benchmark_cs736

## Measurement #1: Precision
Measured rdstsc and clock, with rdtsc being more precise. Script can be run without argument
## Measurement #2: Kernel
```
call 1 - getpid
call 2 - getuid, got a result similar to getpid
call 3 - stat, for file /proc/cpuinfo
```
Script can be run without argument
## Measurement #3: Communication
Due to my mistake, the measurement will be taken on different side for tcp and udp.
For tcp the measurement will be taken on the server, and for udp it be taken on the client

```
./tcp* <host in IP> <port> <test cycle count>
./udp* <host in IP> <port> <test cycle count>
```
