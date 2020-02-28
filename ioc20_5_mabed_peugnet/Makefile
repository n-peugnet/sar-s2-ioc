CC=gcc
NAME=mabed_peugnet
FILES=server.x client2.x ntest.x
CFLAGS=-W -Wall -Wextra -Wfatal-errors -O2

all: $(FILES)

clean:
	rm $(FILES)
	rm *.o

server.x: server.o
	$(CC) -o $@ $^ $(CFLAGS)

client2.x: client2.o
	$(CC) -o $@ $^ $(CFLAGS)

ntest.x: ntest.o
	$(CC) -o $@ $^ -l ncurses

%.o: %.c
	$(CC) -c -o $@ $< $(CFLAGS)