#make file for the c practive program
all: precision kernel socketpair_latency socketpair_throughput tcp_latency_server tcp_latency_client
precision: precision.o benchutil.o
	gcc -Wall -o precision precision.o benchutil.o
kernel: kernel.o benchutil.o
	gcc -Wall -o kernel kernel.o benchutil.o
socketpair_latency: socketpair_latency.o benchutil.o
	gcc -Wall -o socketpair_latency socketpair_latency.o benchutil.o
socketpair_throughput: socketpair_throughput.o benchutil.o
	gcc -Wall -o socketpair_throughput socketpair_throughput.o benchutil.o
tcp_latency_server: tcp_latency_server.o benchutil.o
	gcc -Wall -o tcp_latency_server tcp_latency_server.o benchutil.o
tcp_latency_client: tcp_latency_client.o benchutil.o
	gcc -Wall -o tcp_latency_client tcp_latency_client.o benchutil.o


tcp_latency_server.o: tcp_latency_server.c benchutil.h
	gcc -c tcp_latency_server.c
tcp_latency_client.o: tcp_latency_client.c benchutil.h
	gcc -c tcp_latency_client.c

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
	rm tcp_latency_client
	rm tcp_latency_server
