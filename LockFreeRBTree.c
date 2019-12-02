#include "LockFreeRBTree.h"

#include <linux/export.h>
#include <linux/slab.h>

#define true 1
#define false 0

#define DEBUG(n) printk(#n); printk("====\n");

int search(int value, LockFreeRBNode *root){
	LockFreeRBNode* temp;
	if(root == NULL) return INT_MIN;
	temp = root;
	while(temp != NULL && temp->value >= 0){
		if (value < temp->value) {
			temp = temp->left;
		}else if (value > temp->value) {
			temp = temp->right;
		} else{
			return temp->value;
		}
	}
	return temp==NULL?-1:temp->value;
}

void insert(int value, LockFreeRBNode **root)
{
	LockFreeRBNode *insertedNode, *insertedNodeLeft, *insertedNodeRight;
	LockFreeRBNode *temp1, *temp2;
	
	LockFreeRBNode_constructor(insertedNodeLeft);
	LockFreeRBNode_constructor(insertedNodeRight);
	LockFreeRBNode_constructor_value(insertedNode, insertedNodeLeft, insertedNodeRight, value);
	insertedNode->flag = true;
	
	while(true)
	{
		temp1 = *root;
		temp2 = NULL;
		while(temp1->value >= 0)
		{
			temp2 = temp1;
			if(value<temp1->value)
				temp1 = temp1->left;
			else
				temp1 = temp1->right;
		}
		if(!setupLocalAreaForInsert(temp2))  // todo
		{
			temp2->flag = false;
			continue;
		} 
		else break;
	}
	insertedNode->parent = temp2;
	if(!temp2)
	{
		*root = insertedNode;
	}else if (value < temp2->value){
		temp2->left = insertedNode;
	}else {
		temp2->right = insertedNode;
	}
	insertedNode->left->parent = insertedNode;
	insertedNode->right->parent = insertedNode;
	insertedNode->isRed = true;
	rbInsertFixup(insertedNode, *root); // todo
}

int setupLocalAreaForInsert(LockFreeRBNode *x) {
	LockFreeRBNode *parent, *uncle;
	if(!x){
		return true;
	}
	parent= x->parent;
	if (!x) {
		return true;
	}
	if (!parent) {
		return true;
	}
	if (!__sync_bool_compare_and_swap(&x->flag, false, true)) {
		return false;
	}
	if (!__sync_bool_compare_and_swap(&parent->flag, false, true)) {
		return false;
	}
	if (parent != x->parent) {
		parent->flag = false;
		return false;
	}
	if (x == x->parent->left) {
		uncle = x->parent->right;
	} else {
		uncle = x->parent->left;
	}
	if (uncle && !__sync_bool_compare_and_swap(&uncle->flag, false, true)) {
		x->parent->flag = false;
		return false;
	}
	return true;
}

#define INSERTTOLOCALAREA(V) \
	tempRBnode = kmalloc(sizeof(LockFreeRBNodeList), GFP_KERNEL); \
	tempRBnode->RBNode = (V);\
	list_add_tail(&tempRBnode->list, &local_area)

void rbInsertFixup(LockFreeRBNode *x,  LockFreeRBNode *root)
{
	LockFreeRBNode *temp, *parent, *uncle, *gradparent;
	LockFreeRBNodeList *tempRBnode, *curr_node;
	struct list_head local_area, *tmp;

	uncle = NULL; gradparent = NULL;
	parent = x->parent;

	INIT_LIST_HEAD(&local_area);


	INSERTTOLOCALAREA(x);
	INSERTTOLOCALAREA(parent);

	if (parent != NULL) {
		gradparent = parent->parent;	
	}

	if (gradparent != NULL) {
		if (gradparent->left == parent) {
			uncle = gradparent->right;
		}
		else {
			uncle = gradparent->left;		
		}
	}

	INSERTTOLOCALAREA(uncle);
	INSERTTOLOCALAREA(gradparent);

	while (x->parent && x->parent->isRed) {
		parent = x->parent;
		gradparent = gradparent->parent;

		if (x->parent == x->parent->parent->left) {
			temp = x->parent->parent->right;
			uncle = temp;
			INSERTTOLOCALAREA(x);
			INSERTTOLOCALAREA(parent);
			INSERTTOLOCALAREA(gradparent);
			INSERTTOLOCALAREA(uncle);

			if (temp->isRed) {
				x->parent->isRed = false;
				temp->isRed = false;
				x->parent->parent->isRed = true;
				x = moveLocalAreaUpward(x, &local_area);
			} else {
				if (x == x->parent->right) {
					// Case 2
					x = x->parent;
					leftRotate(&root,x);
				}
				// Case 3
				x->parent->isRed = false;
				x->parent->parent->isRed = true;
				rightRotate(&root,x->parent->parent);
			}
		}
		else {
			temp = x->parent->parent->left;
			uncle = temp;

			INSERTTOLOCALAREA(x);
			INSERTTOLOCALAREA(parent);
			INSERTTOLOCALAREA(gradparent);
			INSERTTOLOCALAREA(uncle);

			if (temp->isRed) {
				// Case 1
				x->parent->isRed = false;
				temp->isRed = false;
				x->parent->parent->isRed = true;
				x = moveLocalAreaUpward(x, &local_area);
			} else {
				if (x == x->parent->left) {
					// Case 2
					x = x->parent;
					rightRotate(&root, x);
				}
				// Case 3
				x->parent->isRed = false;
				x->parent->parent->isRed = true;
				leftRotate(&root, x->parent->parent);
			}
		}
	}

	root->isRed = false;

	list_for_each (tmp, &local_area) {
		curr_node = list_entry(tmp, LockFreeRBNodeList, list);
		if (curr_node->RBNode) curr_node->RBNode->flag = false;
	}
}
#undef INSERTTOLOCALAREA

LockFreeRBNode *moveLocalAreaUpward(LockFreeRBNode *x, struct list_head *working) {
	LockFreeRBNode *parent, *grandparent, *uncle;
	LockFreeRBNode *updated_x, *updated_parent = NULL, *updated_uncle = NULL, *updated_grandparent = NULL;
	LockFreeRBNodeList *updated_x_ListNode, *updated_parent_ListNode, *updated_grandparent_ListNode, *updated_uncle_ListNode;
	parent = x->parent;
	grandparent = parent->parent;

	if (parent == grandparent->left){
		uncle = grandparent->right;
	} else {
		uncle = grandparent->left;
	}

	updated_x = grandparent;

	while (true && updated_x->parent) {
		updated_parent = updated_x->parent;
		if (!__sync_bool_compare_and_swap(&updated_parent->flag, false, true)) {
			continue;
		}
		updated_grandparent = updated_parent->parent;
		if (!updated_grandparent) break;
		if (!__sync_bool_compare_and_swap(&updated_grandparent->flag, false, true)) {
			updated_parent->flag = false;
			continue;
		}
		if (updated_parent == updated_grandparent->left) {
			updated_uncle = updated_grandparent->right;
		} else {
			updated_uncle = updated_grandparent->left;
		}

		if (updated_uncle && !__sync_bool_compare_and_swap(&updated_uncle->flag, false, true)) {
			updated_grandparent->flag = false;
			updated_parent->flag = false;
			continue;
		}
		break;
	}

	updated_x_ListNode = kmalloc(sizeof(LockFreeRBNodeList), GFP_KERNEL);
	updated_x_ListNode->RBNode = updated_x;

	updated_parent_ListNode = kmalloc(sizeof(LockFreeRBNodeList), GFP_KERNEL);
	updated_parent_ListNode->RBNode = updated_parent;

	updated_grandparent_ListNode = kmalloc(sizeof(LockFreeRBNodeList), GFP_KERNEL);
	updated_grandparent_ListNode->RBNode = updated_grandparent;

	updated_uncle_ListNode = kmalloc(sizeof(LockFreeRBNodeList), GFP_KERNEL);
	updated_uncle_ListNode->RBNode = updated_uncle;

	list_add_tail(&updated_x_ListNode->list, working);
	list_add_tail(&updated_parent_ListNode->list, working);
	list_add_tail(&updated_grandparent_ListNode->list, working);
	list_add_tail(&updated_uncle_ListNode->list, working);

	return updated_x;	
}

void leftRotate(LockFreeRBNode **root, LockFreeRBNode *x) {
	LockFreeRBNode *y = x->right;
	if (!x) return;
	x->right = y->left;
	if (y->left) {
		y->left->parent = x;
	}
	y->parent = x->parent;
	if (!x->parent) *root = y;
	else{
		if (x == x->parent->left)
			x->parent->left = y;
		else
			x->parent->right = y;
	}
	y->left = x;
	x->parent = y;
}

void rightRotate(LockFreeRBNode **root, LockFreeRBNode *y) {
	LockFreeRBNode *x = y->left;
	if (!y) return;
	y->left = x->right;
	if (x->right) {
		x->right->parent = y;
	}
	x->parent = y->parent;
	if (!y->parent) *root = x;
	else{
		if (y == y->parent->left)
			y->parent->left = x;
		else
			y->parent->right = x;
	}
	x->right = y;
	y->parent = x;
}

int getheight(LockFreeRBNode *root) {
	int left_height, right_height;
	if (!root)
		return 0;

	left_height = getheight(root->left);
	right_height = getheight(root->right);
	return (left_height > right_height ? left_height : right_height) + 1;
}

void preOrder(LockFreeRBNode *n ){

	if (!n)
		return;
	//n.displayNode(n);
	preOrder(n->left);
	preOrder(n->right);
}

void breadth(LockFreeRBNode *n ){

	if (!n)
		return;
	///n.displayNode(n);
	preOrder(n->left);
	preOrder(n->right);
}

#undef true
#undef false
