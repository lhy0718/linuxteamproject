#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/rbtree.h>
#include <linux/slab.h>
#include <linux/list.h>
#include <linux/time.h>

#define FALSE 0
#define TRUE 1


struct list_head my_list;
struct timespec st, et;
long insert_time;

struct my_type{
	struct rb_node node;
	int key;
	int value;
	int count;
};

struct cachelist_node {
	struct list_head list;
	struct my_type *rb_node;
	int data;
};

/*
   int __init hello_module_init(void)
   {
   struct_example();
   printk("module init\n");
   return 0;
   }

   void __exit hello_module_cleanup(void)
   {
   printk("Bye Module\n");
   }struct timespec st, et;

   module_init(hello_module_init);
   module_exit(hello_module_cleanup);
   */



struct my_type *listSearch(int key){
	struct cachelist_node *current_cache;

	list_for_each_entry(current_cache, &my_list, list){
		if(current_cache->data == key)
			return current_cache -> rb_node;
	}
	return NULL;
}

void countNinsertcache(struct my_type *node){
	node->count+=1;
	printk("%d",node->count);

	if((node->count)>5){
		struct cachelist_node *new = kmalloc(sizeof(struct cachelist_node),GFP_KERNEL);
		new->data = node->key;
		new->rb_node = node;
		list_add(&new->list, &my_list);
	}
}

int rb_insert(struct rb_root *root, struct my_type *data)
{
	struct rb_node **new = &(root->rb_node), *parent = NULL;
	/* Figure out "where" to put new node */
	while (*new) {

		struct my_type *this = container_of(*new, struct my_type, node);
		parent = *new;
		if (this->key > data->key)
			new = &((*new)->rb_left);
		else if (this->key < data->key)
			new = &((*new)->rb_right);
		else
			return FALSE;



	}
	rb_link_node(&data->node, parent, new); /*relinking*/
	rb_insert_color(&data->node, root);
	/*recoloring & rebalancing*/

	return TRUE;
}

struct my_type *rb_search(struct rb_root *root, int key)
{
	struct rb_node *node = root->rb_node;

	struct my_type *returnVal;
	// search success : 1, search fail : -1

	// First, search cache list.
	returnVal = listSearch(key);

	if(returnVal != NULL){
		printk("first search");
		return returnVal;	
	}
	// Else, search like first method.
	while (node) {
		// container_of(address, struct, element which is located in address)
		// start address of struct. search with pointer of sturcture.
		//		struct my_type *data = container_of(node, struct my_type, node);
		struct my_type *data = container_of(node, struct my_type, node);


		if(data->key > key)
			node = node->rb_left;
		else if(data->key < key)
			node = node->rb_right;
		else {
			countNinsertcache(data);
			printk("second search");
			return data;
		}
	}

	return NULL;
}

int rb_delete(struct rb_root *mytree, int key)
{
	struct my_type *data = rb_search(mytree, key);
	if (data) {
		rb_erase(&data->node, mytree);
		kfree(data);
	}
}

void struct_example(void)
{
	/*
	   struct list_head my_list;
	   INIT_LIST_HEAD(&my_list);
	*/
	struct rb_root my_tree = RB_ROOT;
	int i = 0, ret;

	/* rb_node create and insert */
	for(;i<10000;i++){
		struct my_type *new =kmalloc(sizeof(struct my_type),GFP_KERNEL);
		if(!new)
			return NULL;
		new->value = i*10;
		new->key = i;
		new->count = 0;
		ret = rb_insert(&my_tree,new);
	}

	/* rb_tree traversal using iterator */
	struct rb_node *node;
	for (node = rb_first(&my_tree); node; node = rb_next(node))
		//printk("(key,value)=(%d.%d)\n", \
				rb_entry(node, struct my_type, node)->key, \
				rb_entry(node, struct my_type, node)->value);


	for(i=0;i<6;i++){
		getnstimeofday(&st);
		/* rb_tree find node */
		struct my_type *find_node = rb_search(&my_tree,8);
		if(!find_node){
			return NULL;
		}
		getnstimeofday(&et);
        	insert_time=(et.tv_sec-st.tv_sec)*(long)1000000000.0 + (long)(et.tv_nsec-st.tv_nsec);
        	printk("rbtree : %ld ns\n", insert_time);
	        printk("find : (key,value)=(%d.%d)\n", find_node->key,find_node->value);
	
	}
	/* rb_tree delete node */
	rb_delete(&my_tree,0);
}

int __init hello_module_init(void)
{
	//  struct list_head my_list;
	INIT_LIST_HEAD(&my_list);

	struct_example();
	printk("module init\n");
	return 0;
}

void __exit hello_module_cleanup(void)
{
	printk("Bye Module\n");
}

module_init(hello_module_init);
module_exit(hello_module_cleanup);

MODULE_LICENSE("GPL");

