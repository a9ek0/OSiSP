CC = gcc

CFLAGS = -c -W -Wall -Wextra

.PHONY:all
all: start

start:	main.o	
	$(CC) main.o -o main

main.o: src/main.cpp
	$(CC) $(CFLAGS) src/main.cpp
	
clean:
	rm -rf *.o main