obj-m				:= result15.o
result15-objs		:= custom_timer.o LockFreeRBTree.o test.o
KERNEL_DIR			:= /lib/modules/$(shell uname -r)/build 
PWD					:= $(shell pwd)
default :
	make clean
	$(MAKE) -C $(KERNEL_DIR) SUBDIRS=$(PWD) modules
clean :
	$(MAKE) -C $(KERNEL_DIR) SUBDIRS=$(PWD) clean
