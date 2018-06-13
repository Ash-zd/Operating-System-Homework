obj-m := myprocess2.o
myprocess2-objs := myprocess_param.o
KDIR := /lib/modules/$(shell uname -r)/build
PWD := $(shell pwd)
default: 
	make -C $(KDIR) M=$(PWD) modules
clean:
	make -C $(KDIR) M=$(PWD) clean