CC=cc -Wall -Wextra --std=gnu99 -pedantic -Werror

all: head xargs

.PHONY: head
head: head.o
	$(CC) -o $@ head.o

head.o: head.c
	$(CC) -c -o $@ head.c

.PHONY: xargs
xargs: xargs.o
	$(CC) -o $@ xargs.o

xargs.o: xargs.c
	$(CC) -c -o $@ xargs.c

.PHONY: clean
clean:
	rm -f *.o head xargs