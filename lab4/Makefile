CC=cc -Wall -Wextra --std=gnu99 -pedantic -Werror

all: head
.PHONY: head clean

head: head.o
	$(CC) -o $@ head.o

head.o:
	$(CC) -c -o $@ head.c

clean:
	rm -f *.o head