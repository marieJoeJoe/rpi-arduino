PWD := $(shell pwd)
KVERSION := $(shell uname -r)
#KERNEL_DIR := /lib/modules/$(shell uname -r)/build
KERNEL_DIR := /home/pi/ebpf/linux

MODULE_NAME = mini-irq
obj-m := $(MODULE_NAME).o
ccflags-y += -std=gnu99

all:
	make -C $(KERNEL_DIR) M=$(PWD) modules
clean:
	make -C $(KERNEL_DIR) M=$(PWD) clean
