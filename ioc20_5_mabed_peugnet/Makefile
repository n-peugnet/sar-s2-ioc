CC=gcc
NAME=mabed_peugnet
FILES=server.x ntest.x server_dictature.x game_client.x game_server.x
CFLAGS=-W -Wall -Wextra -Wfatal-errors -O2

all: $(FILES)

clean:
	rm *.x
	rm *.o

server.x: server.o
	$(CC) -o $@ $^ $(CFLAGS)

game_client.x: game_client.o
	$(CC) -o $@ $^ $(CFLAGS)

game_server.x: game_server.o
	$(CC) -o $@ $^ $(CFLAGS)

server_dictature.x: server_dictature.o client2.o
	$(CC) -o $@ $^ $(CFLAGS)

ntest.x: ntest.o client2.o
	$(CC) -o $@ $^ -l ncurses

%.o: %.c
	$(CC) -c -o $@ $< $(CFLAGS)