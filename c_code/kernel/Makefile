
CC := gcc
CFLAGS := -Wall -Wextra -std=c99

KERNEL_HEADERS := /lib/modules/$(shell uname -r)/build/include

obj-m += netlink_kernel.o
netlink_kernel-objs := file1.o file2.o
all:
	$(MAKE) -C $(KERNEL_HEADERS) M=$(shell pwd) modules

clean:
	$(MAKE) -C $(KERNEL_HEADERS) M=$(shell pwd) clean