#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/slab.h>
#include "custom_rbtree.h"
#include "custom_timer.h"

#define FALSE 0
#define TRUE 1

TIMER_INIT;

typedef struct{
	struct rb_node node;
	int key;
	int value;
}mytype;

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
	return 0;
}

int cith(void *args){
	struct rb_root *root = ((rd *)args)->root;
	mytype *data = ((rd *)args)->data;
	return 0;
}

int sth(void *args){
	struct rb_root *root = ((rk *)args)->root;
	int key = ((rk *)args)->key;
	return 0;
}

int csth(void *args){
	struct rb_root *root = ((rk *)args)->root;
	int key = ((rk *)args)->key;
	return 0;
}

int dth(void *args){
	struct rb_root *root = ((rk *)args)->root;
	int key = ((rk *)args)->key;
	return 0;
}

int cdth(void *args){
	struct rb_root *root = ((rk *)args)->root;
	int key = ((rk *)args)->key;
	return 0;
}

void test(void){
	struct rb_root control, treatment;
}

int __init _module_init(void){
	test();
	return 0;
}

void __exit _module_cleanup(void){

}

module_init(_module_init);
module_exit(_module_cleanup);
