CC=cc
FLAGS=-Wall -Wextra --std=gnu99 -pedantic
SERVER=server
CLIENT=client

all: shm

shm: shm_server.o shm_client.o lib_client.o lib_server.o
	$(CC) -o $(SERVER) shm_server.o lib_server.o
	$(CC) -o $(CLIENT) shm_client.o lib_client.o

shm_client.o: shm_client.c
	$(CC) $(FLAGS) -c -o $@ $(@:.o=.c)

shm_server.o: shm_server.c
	$(CC) $(FLAGS) -c -o $@ $(@:.o=.c)

lib_client.o: lib_client.c
	$(CC) $(FLAGS) -c -o $@ $(@:.o=.c)

lib_server.o: lib_server.c
	$(CC) $(FLAGS) -c -o $@ $(@:.o=.c)

clean:
	rm -rf *.o $(SERVER) $(CLIENT)