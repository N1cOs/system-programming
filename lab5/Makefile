CC=cc
FLAGS=-Wall -Wextra --std=gnu99 -pedantic
SERVER=server
CLIENT=client
THREAD=threads
PIPE=pipe

all: shm msg th_sem th_sem5 th_mu th_lock sock pipe

shm: shm_server.o shm_client.o lib_client.o lib_server.o
	$(CC) -o $(SERVER) shm_server.o lib_server.o
	$(CC) -o $(CLIENT) shm_client.o lib_client.o

msg: msg_server.o msg_client.o lib_client.o lib_server.o
	$(CC) -o $(SERVER) msg_server.o lib_server.o
	$(CC) -o $(CLIENT) msg_client.o lib_client.o

sock: sock_server.o sock_client.o lib_client.o lib_server.o
	$(CC) -o $(SERVER) sock_server.o lib_server.o
	$(CC) -o $(CLIENT) sock_client.o lib_client.o

sock_client.o: sock_client.c
	$(CC) $(FLAGS) -c -o $@ $(@:.o=.c)

sock_server.o: sock_server.c
	$(CC) $(FLAGS) -g -c -o $@ $(@:.o=.c)
 
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

th_sem: sem_thread.o lib_thread.o
	$(CC) -pthread -o $(THREAD) sem_thread.o lib_thread.o

sem_thread.o: sem_thread.c
	$(CC) $(FLAGS) -c -o $@ $(@:.o=.c)

th_sem5: sem5_thread.o lib_thread.o
	$(CC) -pthread -o $(THREAD) sem5_thread.o lib_thread.o

sem5_thread.o: sem5_thread.c
	$(CC) $(FLAGS) -c -o $@ $(@:.o=.c)

th_mu: mu_thread.o lib_thread.o
	$(CC) -pthread -o $(THREAD) mu_thread.o lib_thread.o

mu_thread.o: mu_thread.c
	$(CC) $(FLAGS) -c -o $@ $(@:.o=.c)

th_lock: lock_thread.o lib_thread.o
	$(CC) -pthread -o $(THREAD) lock_thread.o lib_thread.o

lock_thread.o: lock_thread.c
	$(CC) $(FLAGS) -c -o $@ $(@:.o=.c)

lib_thread.o: lib_thread.c
	$(CC) $(FLAGS) -c -o $@ $(@:.o=.c)

pipe: pipe.o
	$(CC) -o $(PIPE) pipe.o

pipe.o: pipe.c
	$(CC) $(FLAGS) -c -o $@ $(@:.o=.c)

clean:
	rm -rf *.o $(SERVER) $(CLIENT) $(THREAD) $(PIPE)