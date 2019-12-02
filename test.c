#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/kthread.h>
#include <linux/slab.h>
#include <linux/rbtree.h>

#include "LockFreeRBTree.h"
#include "custom_timer.h"

#define SIZE 100000
#define NUM_OF_THREAD 10


typedef struct{
	int value;
	struct rb_node node;
}mynode;

typedef struct{
	int value;
	LockFreeRBNode *root;
} args;

int insert_thread(void *arg){
	int value, loop;
	LockFreeRBNode *root;
	args tmp = *(args *)arg;
	
	TIMER_INIT;
	
	value = tmp.value;
	root = tmp.root;
	
	TIMER_START;
	for(loop=value; loop < SIZE; loop += NUM_OF_THREAD)
		insert(value, &root);
	TIMER_END;

	printk("thread %d: %ld ns\n", value, TIMER);
	return 0;
}

int rb_insert(struct rb_root *root, mynode *data) {
	struct rb_node **new = &(root->rb_node), *parent = NULL;
	/* Figure out "where" to put new node */ 
	while (*new) {
		mynode *this = container_of(*new, mynode, node);
		parent = *new;
		if (this->value > data->value)
			new = &((*new)->rb_left); 
		else if (this->value < data->value)
			new = &((*new)->rb_right);
		else
		       return false;	
	}
	rb_link_node(&data->node, parent, new); 
	rb_insert_color(&data->node, root);
	return true;
}

void set_and_run_insert(int val, LockFreeRBNode *root){ 
	args *tmp = kmalloc(sizeof(args), GFP_KERNEL);
	tmp->root = root;
	tmp->value = val;
	kthread_run(&insert_thread, tmp, "thread");
}

void test(void){
	int loop;
	LockFreeRBNode *lock_free_root;
	struct rb_root rb_root;
	mynode *mnode;
	TIMER_INIT;

	LockFreeRBNode_constructor(lock_free_root);
	rb_root = RB_ROOT;

	TIMER_START;
	for(loop=0; loop<SIZE; loop++){
		mnode = kmalloc(sizeof(mynode), GFP_KERNEL);
		if(!mnode){
			printk("CANT ALLOCATE!\n");
			break;
		}
		mnode->value = loop;
		rb_insert(&rb_root, mnode);
	}
	TIMER_END;
	printk("rb tree timer: %ld ns\n", TIMER);

	set_and_run_insert(0, lock_free_root);
	set_and_run_insert(1, lock_free_root);
	set_and_run_insert(2, lock_free_root);
	set_and_run_insert(3, lock_free_root);
	set_and_run_insert(4, lock_free_root);
	set_and_run_insert(5, lock_free_root);
	set_and_run_insert(6, lock_free_root);
	set_and_run_insert(7, lock_free_root);
	set_and_run_insert(8, lock_free_root);
	set_and_run_insert(9, lock_free_root);
}

int __init _module_init(void){
	printk("=============module start=============\n");
	test();
	return 0;
}

void __exit _module_cleanup(void){
}

module_init(_module_init);
module_exit(_module_cleanup);
MODULE_LICENSE("GPL");
