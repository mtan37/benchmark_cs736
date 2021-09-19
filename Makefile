#make file for the c practive program
run: precision.o
	gcc -Wall -o precision precision.o
precision.o: precision.c
	gcc -c precision.c
clean:
	rm *.o
	rm run
