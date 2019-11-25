#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/slab.h>
#include "custom_rbtree.h"
#include "custom_timer.h"

#define FALSE 0
#define TRUE 1

typedef struct{
	struct rb_node node;
	int key;
	int value;
}mytype;

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

int rb_delete(struct rb_root *mytree, int key){
	mytype *data = rb_search(mytree, key);
	if(data){
		rb_erase(&data->node, mytree);
		kfree(data);
		return TRUE;
	}
	return FALSE;
}

void test(void){
	TIMER_START;
	TIMER_END;
	printk("timer : %ld\n", TIMER); 
}

int __init _module_init(void){
	test();
	return 0;
}

void __exit _module_cleanup(void){

}

module_init(_module_init);
module_exit(_module_cleanup);
