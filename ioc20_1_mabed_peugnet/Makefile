CC=bcm2708hardfp-gcc
CFLAGS=-O2 -static
CARD=23
NAME=mabed_peugnet
CFLAGS=-W -Wall -Wextra -Wfatal-errors -O2
APP=helloworld
PATH:=/users/enseig/franck/IOC/arm-bcm2708hardfp-linux-gnueabi/bin:$(PATH)

all: $(APP)
	scp -P 622$(CARD) $^ pi@peri:$(NAME)

clean:
	rm $(APP)

helloworld: helloworld.c
	$(CC) -o $@ $< $(CFLAGS)