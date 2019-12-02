#ifndef	_LINUX_LOCKFREE_RBTREE_H
#define	_LINUX_LOCKFREE_RBTREE_H

#include <linux/kernel.h>
#include <linux/stddef.h>
#include <linux/list.h>
#include <linux/rcupdate.h>

#define LockFreeRBNode_constructor(name) name = kmalloc(sizeof(LockFreeRBNode), GFP_KERNEL);\
	name->value = INT_MIN;\
	name->left = NULL;\
	name->right = NULL;\
	name->parent = NULL;\
	name->isRed = false;\
	name->flag = false;

#define LockFreeRBNode_constructor_value(name, left_node, right_node, _value) name = kmalloc(sizeof(LockFreeRBNode), GFP_KERNEL);\
	 name->value = (_value);\
	 name->left = left_node;\
	 name->right = right_node;\
	 name->parent = NULL;\
	 name->isRed = true;\
	 name->flag = false;

struct LockFreeRBNode{
  int value;
  struct LockFreeRBNode* left;
  struct LockFreeRBNode* right;
  struct LockFreeRBNode* parent;
  int isRed;
  int flag;
};

typedef struct LockFreeRBNode LockFreeRBNode;

struct LockFreeRBNodeList{
  struct list_head list;
  LockFreeRBNode* RBNode;
};

typedef struct LockFreeRBNodeList LockFreeRBNodeList;

int search(int value, LockFreeRBNode *root);

void insert(int value, LockFreeRBNode **root);

int setupLocalAreaForInsert(LockFreeRBNode *x);

void rbInsertFixup(LockFreeRBNode *x,  LockFreeRBNode *root);

LockFreeRBNode *moveLocalAreaUpward(LockFreeRBNode *x, struct list_head *working);

void leftRotate(LockFreeRBNode **root, LockFreeRBNode *x);

void rightRotate(LockFreeRBNode **root, LockFreeRBNode *y);

int getheight(LockFreeRBNode *root);

void preOrder(LockFreeRBNode *n );

void breadth(LockFreeRBNode *n );

#endif
