CARD_NUMB       = 24
ROUTER          = peri
LOGIN           = mabed_peugnet
LAB             = lab2


########################### module vars ###################################
MODULE          = MPledbp

CROSSDIR        = /users/enseig/franck/IOC
KERNELDIR       = /dsk/l1/misc/linux-rpi-3.18.y2
CROSS_COMPILE   = $(CROSSDIR)/arm-bcm2708hardfp-linux-gnueabi/bin/bcm2708hardfp-

obj-m           += $(MODULE).o

############################# main vars ###################################
#CFLAGS=-Wall -Wfatal-errors -O2
LDFLAGS=-static -pthread

PATH:=/users/enseig/franck/IOC/arm-bcm2708hardfp-linux-gnueabi/bin:$(PATH)

############################### targets ###################################
default: main.x
	make -C $(KERNELDIR) ARCH=arm CROSS_COMPILE=$(CROSS_COMPILE) M=$(PWD) modules

clean:
	make -C $(KERNELDIR) ARCH=arm CROSS_COMPILE=$(CROSS_COMPILE) M=$(PWD) clean
	rm -f *.o *.x *~ *.a
upload:
	scp -P622$(CARD_NUMB) $(MODULE).ko pi@$(ROUTER):$(LOGIN)/$(LAB)
	scp -P622$(CARD_NUMB) insdev pi@$(ROUTER):$(LOGIN)/$(LAB)
	scp -P622$(CARD_NUMB) rmdev pi@$(ROUTER):$(LOGIN)/$(LAB)
	scp -P622$(CARD_NUMB) MPledbp.sh pi@$(ROUTER):$(LOGIN)/$(LAB)
	scp -P622$(CARD_NUMB) main.x pi@$(ROUTER):$(LOGIN)/$(LAB)

%.x: %.c
	which $(CROSS_COMPILE)gcc
	$(CROSS_COMPILE)gcc -o $@ $^ $(LDFLAGS)