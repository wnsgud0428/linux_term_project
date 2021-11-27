	#--------- Makefile ---------#
obj-m := my_module.o

KERNER_DIR := /lib/modules/5.10.70TaeGyunHan/build
PWD := $(shell pwd)

default:
	$(MAKE) -C $(KERNER_DIR) M=$(PWD) modules
clean:
	$(MAKE) -C $(KERNER_DIR) M=$(PWD) clean
