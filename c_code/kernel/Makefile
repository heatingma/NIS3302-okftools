CC := gcc
CFLAGS := -Wall -Wextra -std=c99 -O2 -g

KERNEL_HEADERS := /lib/modules/$(shell uname -r)/build

obj-m += netlink_kernel.o

all:
	$(MAKE) -C $(KERNEL_HEADERS) M=$(shell pwd) CFLAGS="$(CFLAGS)" modules

clean:
	$(MAKE) -C $(KERNEL_HEADERS) M=$(shell pwd) clean