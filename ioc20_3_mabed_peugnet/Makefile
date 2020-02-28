# changer les valeurs par défaut de RPI et DIR
RPI?=26
DIR?=mabed_peugnet
APP=MPlcd_user
ROUTER=peri

# CFLAGS=-Wall -Wfatal-errors -O2
LDFLAGS=-static -L$(LIB) -lgpio

KERNELDIR       = /dsk/l1/misc/linux-rpi-3.18.y
CROSSDIR        = /users/enseig/franck/IOC
CROSS_COMPILE   = $(CROSSDIR)/arm-bcm2708hardfp-linux-gnueabi/bin/bcm2708hardfp-


########################### module vars ###################################
MODULE          = MPlcd

obj-m           += $(MODULE).o

############################# main vars ###################################
#CFLAGS=-Wall -Wfatal-errors -O2
LDFLAGS=-static -pthread

PATH:=/users/enseig/franck/IOC/arm-bcm2708hardfp-linux-gnueabi/bin:$(PATH)

############################### targets ###################################
.PHONY: all
all: $(APP).x main.x
	make -C $(KERNELDIR) ARCH=arm CROSS_COMPILE=$(CROSS_COMPILE) M=$(PWD) modules

%.x: %.c
	$(CROSS_COMPILE)gcc -o $@ $< -I$(INC) -O2 -static

.PHONY: upload
upload: all
	scp -P622$(RPI) $(APP).x pi@$(ROUTER):$(DIR)
	scp -P622$(RPI) main.x pi@$(ROUTER):$(DIR)
	scp -P622$(RPI) $(MODULE).ko pi@$(ROUTER):$(DIR)
	scp -P622$(RPI) insdev pi@$(ROUTER):$(DIR)
	scp -P622$(RPI) rmdev pi@$(ROUTER):$(DIR)

.PHONY: clean
clean:
	make -C $(KERNELDIR) ARCH=arm CROSS_COMPILE=$(CROSS_COMPILE) M=$(PWD) clean
	rm -f *.o *.x *~