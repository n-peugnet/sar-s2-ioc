NAME = mabed_peugnet
RPI ?= 24

CROSS_COMPILE ?= bcm2708hardfp-

CFLAGS=-Wall -Wfatal-errors -O2
LDFLAGS=-static -pthread

PATH:=/users/enseig/franck/IOC/arm-bcm2708hardfp-linux-gnueabi/bin:$(PATH)
#PATH:=../bin:$(PATH)

help:
	@echo ""
	@echo "make [RPI=num] target"
	@echo ""
	@echo "      target :	help	this message"
	@echo " 		all	clean all and recompile"
	@echo " 		dep	compile if required"
	@echo " 		clean	clean all except sources"
	@echo " 		copy	compile then copy to card $(RPI) in directory $(NAME)"
	@echo ""

all: clean dep 

dep : blink0.x blink0_pt.x blink01_pt.x blink01_bp_pt.x read_bp.x

copy: *.x
	scp -P 622$(RPI) $^ pi@peri:$(NAME)

%.x: %.c
	which $(CROSS_COMPILE)gcc
	$(CROSS_COMPILE)gcc -o $@ $^ $(LDFLAGS)

clean:
	rm -f *.o *.x *~ *.a
