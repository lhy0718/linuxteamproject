#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/kthread.h>
#include <linux/slab.h>

#include "LockFreeRBTree.h"
#include "custom_timer.h"


void test(void){
	int loop;
	LockFreeRBNode *root;

	TIMER_INIT;

	root = kmalloc(sizeof(LockFreeRBNode), GFP_KERNEL);
	root->value = INT_MIN;
	root->left = NULL;
	root->right = NULL;
	root->parent = NULL;
	root->isRed = false;
	root->flag = false;

	TIMER_START;
	for(loop=0; loop<1000; loop++){
		insert(loop, &root);
	}
	TIMER_END;
	printk("%ld ns\n", TIMER);
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
