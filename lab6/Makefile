CC=cc
FLAGS=-Wall -Wextra --std=gnu99 -pedantic -g
SERVER=server
CLIENT=client

lab6: server client

client: client.o
	$(CC) $(FLAGS) -o $(CLIENT) client.o

server: server.o dirs_parse.o dirs_handle.o logs.o
	$(CC) $(FLAGS) -pthread -o $(SERVER) server.o dirs_parse.o dirs_handle.o logs.o

server.o: server.c
	$(CC) $(FLAGS) -c -o $@ $(@:.o=.c)

logs.o: logs.c
	$(CC) $(FLAGS) -c -o $@ $(@:.o=.c)

dirs_parse.o: dirs_parse.c
	$(CC) $(FLAGS) -c -o $@ $(@:.o=.c)

dirs_handle.o: dirs_handle.c
	$(CC) $(FLAGS) -c -o $@ $(@:.o=.c)

client.o: client.c
	$(CC) $(FLAGS) -c -o $@ $(@:.o=.c)

clean:
	rm -rf *.o $(SERVER) $(CLIENT)