#make file for the c practive program
run: main.o
	gcc -Wall -o run main.o
main.o: main.c
	gcc -c main.c
clean:
	rm *.o
	rm run
