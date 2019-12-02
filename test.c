#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/kthread.h>
#include <linux/slab.h>
#include <linux/rbtree.h>

#include "LockFreeRBTree.h"
#include "custom_timer.h"

typedef struct{
	int value;
	LockFreeRBNode *root;
} args;

int insert_thread(void *arg){
	int value;
	LockFreeRBNode *root;

	value = ((args *)arg)->value;
	root = ((args *)arg)->root;

	insert(value, &root);
	return 0;
}

void test(void){
	int loop;
	LockFreeRBNode *root;
	args *arguments;
	TIMER_INIT;

	LockFreeRBNode_constructor(root);
	
	TIMER_START;
	for(loop=0; loop<1000; loop++){
		arguments = kmalloc(sizeof(args), GFP_KERNEL);
		arguments->value = loop;
		arguments->root = root;
		kthread_run(&insert_thread, arguments, "thread");
		kfree(arguments);
	}
	TIMER_END;
	printk("timer: %ld ns", TIMER);

	TIMER_START;

	TIMER_END;
	printk("timer: %ld ns", TIMER);
}

int __init _module_init(void){
	test();
	return 0;
}

void __exit _module_cleanup(void){
}

module_init(_module_init);
module_exit(_module_cleanup);
MODULE_LICENSE("GPL");
