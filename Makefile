#make file for the c practive program
all: precision kernel socketpair
precision: precision.o
	gcc -Wall -o precision precision.o
kernel: kernel.o
	gcc -Wall -o kernel kernel.o
socketpair: socketpair.o
	gcc -Wall -o socketpair socketpair.o

socketpair.o: socketpair.c
	gcc -c socketpair.c
kernel.o: kernel.c
	gcc -c kernel.c
precision.o: precision.c
	gcc -c precision.c
clean:
	rm *.o
	rm precision
	rm kernel
