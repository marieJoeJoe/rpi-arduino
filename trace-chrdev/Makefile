
obj-m := trace_chrdev.o

PWD := $(shell pwd)
KDIR:= /home/pi/ebpf/linux



all:
	make -C $(KDIR) M=$(PWD)
clean:
	rm -rf *.o *.ko *.mod.c *.symvers *.c~ *~
