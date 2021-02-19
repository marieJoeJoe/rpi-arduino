
obj-m := trace_chrdev.o

PWD := $(shell pwd)
KDIR:= /lib/modules/4.4.0-31-generic/build

all:
	make -C $(KDIR) M=$(PWD)
clean:
	rm -rf *.o *.ko *.mod.c *.symvers *.c~ *~