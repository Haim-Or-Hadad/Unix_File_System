all: test

test: test_createfile.o file_system.o 
		gcc -o test test_createfile.o file_system.o

test.o: test_createfile.c file_system.h
		gcc -c test.c

file_system.o: file_system.c file_system.h
		gcc -c file_system.c

clean: 
	rm -f  test  file_system.o test_createfile.o