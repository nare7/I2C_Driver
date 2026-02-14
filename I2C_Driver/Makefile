# Makefile for I2C Character Driver

obj-m += i2c_char_driver.o

KERNEL_DIR ?= /lib/modules/$(shell uname -r)/build
PWD := $(shell pwd)

all:
	make -C $(KERNEL_DIR) M=$(PWD) modules

clean:
	make -C $(KERNEL_DIR) M=$(PWD) clean

install:
	sudo insmod i2c_char_driver.ko

uninstall:
	sudo rmmod i2c_char_driver

logs:
	dmesg | tail -20

help:
	@echo "Makefile for I2C Character Driver"
	@echo ""
	@echo "Usage:"
	@echo "  make          - Build the kernel module"
	@echo "  make clean    - Clean build files"
	@echo "  make install  - Insert the module into kernel"
	@echo "  make uninstall- Remove the module from kernel"
	@echo "  make logs     - Show kernel logs (dmesg)"
