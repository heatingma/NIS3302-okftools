CFLAGS += -std=c11 -Wall -Wextra -O2 -g
obj-m += netlink_kernel.o
all:
	make -C /lib/modules/$(shell uname -r)/build M=$(shell pwd) modules
clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(shell pwd) clean
