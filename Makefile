obj-m				:= result3.o
result3-objs		:= custom_timer.o LockFreeRBTree.o test.o
KERNEL_DIR			:= /lib/modules/$(shell uname -r)/build 
PWD					:= $(shell pwd)
default :
	$(MAKE) -C $(KERNEL_DIR) SUBDIRS=$(PWD) modules
clean :
	$(MAKE) -C $(KERNEL_DIR) SUBDIRS=$(PWD) clean
