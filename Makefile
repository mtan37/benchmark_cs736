#make file for the c practive program
all: precision kernel socketpair
precision: precision.o benchutil.o
	gcc -Wall -o precision precision.o benchutil.o
kernel: kernel.o benchutil.o
	gcc -Wall -o kernel kernel.o benchutil.o
socketpair: socketpair.o benchutil.o
	gcc -Wall -o socketpair socketpair.o benchutil.o

socketpair.o: socketpair.c
	gcc -c socketpair.c
kernel.o: kernel.c
	gcc -c kernel.c
precision.o: precision.c
	gcc -c precision.c
benchutil.o: benchutil.c
	gcc -c benchutil.c
clean:
	rm *.o
	rm precision
	rm kernel
