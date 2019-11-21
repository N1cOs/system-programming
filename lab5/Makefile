CC=cc
FLAGS=-Wall -Wextra --std=gnu99 -pedantic
SERVER=server
CLIENT=client

all: shm msg

shm: shm_server.o shm_client.o lib_client.o lib_server.o
	$(CC) -o $(SERVER) shm_server.o lib_server.o
	$(CC) -o $(CLIENT) shm_client.o lib_client.o

msg: msg_server.o msg_client.o lib_client.o lib_server.o
	$(CC) -o $(SERVER) msg_server.o lib_server.o
	$(CC) -o $(CLIENT) msg_client.o lib_client.o
 
msg_client.o: msg_client.c
	$(CC) $(FLAGS) -c -o $@ $(@:.o=.c)

msg_server.o: msg_server.c
	$(CC) $(FLAGS) -g -c -o $@ $(@:.o=.c)

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