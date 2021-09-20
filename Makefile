#make file for the c practive program
all: precision kernel socketpair
precision: precision.o benchutil.o
	gcc -Wall -o precision precision.o benchutil.o
kernel: kernel.o benchutil.o
	gcc -Wall -o kernel kernel.o benchutil.o
socketpair: socketpair.o benchutil.o
	gcc -Wall -o socketpair socketpair.o benchutil.o

socketpair.o: socketpair.c benchutil.h
	gcc -c socketpair.c
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
	rm socketpair
