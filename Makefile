obj-m					:= project.o
project-objs	:= custom_rbtree.o custom_timer.o hanyong.o
KERNEL_DIR		:= /lib/modules/$(shell uname -r)/build 
PWD						:= $(shell pwd)
default :
	$(MAKE) -C $(KERNEL_DIR) SUBDIRS=$(PWD) modules
clean :
	$(MAKE) -C $(KERNEL_DIR) SUBDIRS=$(PWD) clean
