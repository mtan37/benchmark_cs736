#make file for the c practive program
all: precision kernel socketpair_latency socketpair_throughput
precision: precision.o benchutil.o
	gcc -Wall -o precision precision.o benchutil.o
kernel: kernel.o benchutil.o
	gcc -Wall -o kernel kernel.o benchutil.o
socketpair_latency: socketpair_latency.o benchutil.o
	gcc -Wall -o socketpair_latency socketpair_latency.o benchutil.o
socketpair_throughput: socketpair_throughput.o benchutil.o
	gcc -Wall -o socketpair_throughput socketpair_throughput.o benchutil.o

socketpair_latency.o: socketpair_latency.c benchutil.h
	gcc -c socketpair_latency.c
socketpair_throughput.o: socketpair_throughput.c benchutil.h
	gcc -c socketpair_throughput.c
kernel.o: kernel.c benchutil.h
	gcc -c kernel.c
precision.o: precision.c benchutil.h
	gcc -c precision.c
benchutil.o: benchutil.c benchutil.h
	gcc -c benchutil.c
clean:
	rm *.o
	rm precision
	rm kernel
	rm socketpair_latency
	rm socketpair_throughput
