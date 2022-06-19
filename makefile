.PHONY: all clean

all: test1 test2


test1: test1.o file_system.o 
	gcc -g -o test1 test1.o file_system.o
test1.o: test1.c file_system.h
	gcc -g -c test1.c
file_system.o: file_system.c file_system.h
	gcc -g -c file_system.c

test2: test2.o Mylibc.o file_system.o
	gcc -g -o test2 test2.o Mylibc.o file_system.o
test2.o: test2.c Mylibc.h
	gcc -g -c test2.c
Mylibc.o: Mylibc.c Mylibc.h
	gcc -g -c Mylibc.c

clean:
	rm -f *.o test1 fs_data test2
    