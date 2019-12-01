#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/kthread.h>
#include <linux/delay.h>
#include "custom_rbtree.h"
#include "custom_timer.h"

#define FALSE 0
#define TRUE 1

spinlock_t lock;

typedef struct{
	struct rb_node node;
	int key;
	int value;
}mytype;

//thread arguments
typedef struct{
	struct rb_root *root;
	mytype *data;
}rd;

typedef struct{
	struct rb_root *root;
	int key;
}rk;

int rb_insert(struct rb_root *root, mytype *data){
	struct rb_node **new = &(root->rb_node), *parent = NULL;

	while(*new){
		mytype *this = container_of(*new, mytype, node);
		parent = *new;
		if(this->key > data->key)
			new = &((*new)->rb_left);
		else if(this->key < data->key)
			new = &((*new)->rb_right);
		else
			return FALSE;
	}

	rb_link_node(&data->node, parent, new);
	rb_insert_color(&data->node, root);

	return TRUE;
}

int custom_rb_insert(struct rb_root *root, mytype *data){
	struct rb_node **new = &(root->rb_node), *parent = NULL;

	while(*new){
		mytype *this = container_of(*new, mytype, node);
		parent = *new;
		if(this->key > data->key)
			new = &((*new)->rb_left);
		else if(this->key < data->key)
			new = &((*new)->rb_right);
		else
			return FALSE;
	}

	rb_link_node(&data->node, parent, new);
	rb_insert_color(&data->node, root);

	return TRUE;
}

mytype *rb_search(struct rb_root *root, int key){
	struct rb_node *node = root->rb_node;

	while(node){
		mytype *data = container_of(node, mytype, node);
		if(data->key > key)
			node = node->rb_left;
		else if(data->key < key)
			node = node->rb_right;
		else
			return data;
	}
	return NULL;
}

mytype *custom_rb_search(struct rb_root *root, int key){
	struct rb_node *node = root->rb_node;
	
	while(node){
		mytype *data = container_of(node, mytype, node);
		if(data->key > key)
			node = node->rb_left;
		else if(data->key < key)
			node = node->rb_right;
		else
			return data;
	}
	return NULL;
}

int rb_delete(struct rb_root *mytree, int key){
	mytype *data = rb_search(mytree, key);
	if(data){
		rb_erase(&data->node, mytree);
		kfree(data);
		return TRUE;
	}
	return FALSE;
}

int custom_rb_delete(struct rb_root *mytree, int key){
	mytype *data = custom_rb_search(mytree, key);
	if(data){
		rb_erase(&data->node, mytree);
		kfree(data);
		return TRUE;
	}
	return FALSE;
}

int ith(void *args){
	struct rb_root *root = ((rd *)args)->root;
	mytype *data = ((rd *)args)->data;
	rb_insert(root, data);
	return 0;
}

int cith(void *args){
	struct rb_root *root = ((rd *)args)->root;
	mytype *data = ((rd *)args)->data;
	custom_rb_insert(root, data);
	return 0;
}

int sth(void *args){
	struct rb_root *root = ((rk *)args)->root;
	int key = ((rk *)args)->key;
	rb_search(root, key);
	return 0;
}

int csth(void *args){
	struct rb_root *root = ((rk *)args)->root;
	int key = ((rk *)args)->key;
	TIMER_INIT;
	TIMER_START;
	custom_rb_search(root, key);
	TIMER_END;
	printk("thread %ld ns\n", TIMER);
	return 0;
}

int dth(void *args){
	struct rb_root *root = ((rk *)args)->root;
	int key = ((rk *)args)->key;
	rb_delete(root, key);
	return 0;
}

int cdth(void *args){
	struct rb_root *root = ((rk *)args)->root;
	int key = ((rk *)args)->key;
	custom_rb_delete(root, key);
	return 0;
}

void test(void){
	struct rb_root control = RB_ROOT, treatment = RB_ROOT;
	int loop;
	rk *search_target = kmalloc(sizeof(rk), GFP_KERNEL);
	TIMER_INIT;
	for(loop = 0; loop < 100000; loop++){
		mytype *new = kmalloc(sizeof(mytype), GFP_KERNEL);
		if (!new)
			break;
		new->value = loop;
		new->key = loop;
		rb_insert(&control, new);
	}
	for(loop = 0; loop < 100000; loop++){
		mytype *new = kmalloc(sizeof(mytype), GFP_KERNEL);
		if (!new)
			break;
		new->value = loop;
		new->key = loop;
		rb_insert(&treatment, new);
	}

	for(loop=0; loop<10; loop++){
		TIMER_START;
		rb_search(&control, 100000);
		TIMER_END;
		printk("control : %ld ns\n", TIMER);
	}
	for(loop = 0; loop < 100000; loop){
		rb_delete
	}
	search_target->root = &treatment;
	search_target->key = 100000;
	printk("asdf\n");
	//for(loop=0; loop<10; loop++){
		kthread_run(&csth, search_target, "thread");
	//	mdelay(100);
	//}
}

int __init _module_init(void){
	test();
	return 0;
}

void __exit _module_cleanup(void){

}

module_init(_module_init);
module_exit(_module_cleanup);
