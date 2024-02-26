CC = gcc

CFLAGS = -c -W -Wall -Wno-unused-parameter -Wno-unused-variable -std=c11 -pedantic

.PHONY:all
all: start

start:	parent.o child.o
	$(CC) parent.o -o parent
	$(CC) child.o -o child
	
parent.o: src/parent.c
	$(CC) $(CFLAGS) src/parent.c
	
child.o: src/child.c
	$(CC) $(CFLAGS) src/child.c

clean:
	rm -rf *.o src/parent src/child