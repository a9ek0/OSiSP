CC = gcc

CFLAGS = -W -Wall -Wno-unused-parameter -Wno-unused-variable -std=c11 -pedantic

.PHONY:all
all: start

start:	parent.o child_00.o
	$(CC) parent.o -o parent
	$(CC) child_00.o -o child_00
	
parent.o: parent.c
	$(CC) $(CFLAGS) parent.c
	
child_00.o: child_00.c
	$(CC) $(CFLAGS) child_00.c

clean:
	rm -rf *.o parent child	