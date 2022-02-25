/*
	Basic inspiration for the Fib Heap approach can be attributed to

	https://setscholars.net/algorithm-in-c-fibonacci-heap/

	However, the implementation of consolidate was particularly wrong (as was the degree
	calculation), and there were several aspects of the fib_heap/fib_node structs that had 
	to be changed to meet the specific use case.
*/


#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

typedef struct fib_node {
	float key;
	int degree;
	int vert_id;
	struct fib_node *left_sibling;
	struct fib_node *right_sibling;
	struct fib_node *parent;
	struct fib_node *child;
	bool mark;
	bool visited;
} NODE;

typedef struct fib_heap {
	int n;
	NODE *min;
	int phi;
	int degree;
} FIB_HEAP;

FIB_HEAP *make_fib_heap();
NODE* insertion(FIB_HEAP *H, NODE *new, float val,int vert_id);
NODE *extract_min(FIB_HEAP *H);
void consolidate(FIB_HEAP *H);
void fib_heap_link(FIB_HEAP *H, NODE* y, NODE *x);
void decrease_key(FIB_HEAP *H,NODE *node, float key);
void cut(FIB_HEAP *H, NODE *node_to_be_decrease, NODE *parent_node);
void cascading_cut(FIB_HEAP *H, NODE *parent_node);
void delete_node(FIB_HEAP *H, float key);

FIB_HEAP *make_fib_heap(){
	
	FIB_HEAP *H = (FIB_HEAP *)malloc(sizeof(FIB_HEAP));
	H->n = 0;
	H->min = NULL;
	H->phi = 0;
	H->degree = 0;
	return H;
}

NODE* insertion(FIB_HEAP *H, NODE *new, float val, int vert_id) {
	new = (NODE *)malloc(sizeof(NODE));
	new->key = val;
	new->vert_id = vert_id;
	new->degree = 0;
	new->mark = false;
	new->parent = NULL;
	new->child = NULL;
	new->visited = false;
	new->left_sibling = new;
	new->right_sibling = new;
	if (H->min == NULL) {
		H->min = new;
	}
	else {
		H->min->left_sibling->right_sibling = new;
		new->right_sibling = H->min;
		new->left_sibling = H->min->left_sibling;
		H->min->left_sibling = new;
		if (new->key < H->min->key) {
			H->min = new;
		}
	}
	(H->n)++;

	return new;
}

// Bound it by golden (actually log 1.5, little looser, because meh)
int cal_degree(int n) {
	return floor(log(n)/(log(1.61803)));
}

void consolidate(FIB_HEAP *H) {
	int degree,i,d;
	degree = cal_degree(H->n);
	NODE *A[degree+1], *x, *y, *z;
	for (i = 0; i <= degree; i++) {
		A[i] = NULL;
	}
	x = H->min;
	bool breakFlag = false;

	while (true) {
		d = x->degree;
		while (A[d] != NULL) {
			y = A[d];
			if (y == x) {
				breakFlag = true;
				break;
			}
			if (x->key > y->key) {
				NODE* tmp = x;
				x = y;
				y = tmp;
			}
			fib_heap_link(H,y,x);
			A[d] = NULL;
			d++;
		}
		if (breakFlag)
			break;
		A[x->degree] = x;
		x = x->right_sibling;
	}


	H->min = x;
	NODE* iter = x;
	do {
		if (iter-> key < H->min->key) {
			H->min = iter;
		}
		iter = iter->right_sibling;
	} while(iter != x);
}

void fib_heap_link(FIB_HEAP *H, NODE *y, NODE *x) {
	y->right_sibling->left_sibling = y->left_sibling;
	y->left_sibling->right_sibling = y->right_sibling;

	if (x->right_sibling == x)
		H->min =x;

	y->left_sibling = y;
	y->right_sibling = y;
	y->parent = x;

	if (x->child == NULL) {
		x->child = y;
	}
	y->right_sibling = x->child;
	y->left_sibling = x->child->left_sibling;
	x->child->left_sibling->right_sibling = y;
	x->child->left_sibling = y;
	if ((y->key) < (x->child->key))
		x->child = y;

	(x->degree)++;
}

NODE *extract_min(FIB_HEAP *H) {
	if (H->min != NULL) {
		NODE *temp = H->min;
		NODE *pntr;
		pntr = temp;
		NODE *x = NULL;
		if (temp->child != NULL) {
			x = temp->child;
			do {
				pntr = x->right_sibling;
				(H->min->left_sibling)->right_sibling = x;
				x->right_sibling = H->min;
				x->left_sibling = H->min->left_sibling;
				H->min->left_sibling = x;
				if (x->key < H->min->key)
					H->min = x;
				x->parent = NULL;
				x = pntr;
			} while (pntr != temp->child);
		}

		(temp->left_sibling)->right_sibling = temp->right_sibling;
		(temp->right_sibling)->left_sibling = temp->left_sibling;
		H->min = temp->right_sibling;

		if (temp == temp->right_sibling && temp->child == NULL)
			H->min = NULL;
		else {
			H->min = temp->right_sibling;
			consolidate(H);
		}
		H->n = H->n - 1;
		return temp;
	}
	return H->min;
}

void cut(FIB_HEAP *H, NODE* to_decrease, NODE* parent_node) {
	NODE* temp_parent_check;

	if (to_decrease == to_decrease->right_sibling)
		parent_node->child = NULL;

	to_decrease->left_sibling->right_sibling = to_decrease->right_sibling;
	to_decrease->right_sibling->left_sibling = to_decrease->left_sibling;
	if (to_decrease == parent_node->child)
		parent_node->child = to_decrease->right_sibling;

	(parent_node->degree)--;

	to_decrease->left_sibling = to_decrease;
	to_decrease->right_sibling = to_decrease;
	H->min->left_sibling->right_sibling = to_decrease;
	to_decrease->right_sibling = H->min;
	to_decrease->left_sibling = H->min->left_sibling;
	H->min->left_sibling = to_decrease;

	to_decrease->parent = NULL;
	to_decrease->mark = false;
}

void cascading_cut(FIB_HEAP *H, NODE *parent_node){
	NODE *aux;
	aux = parent_node->parent;
	if (aux != NULL) {
		if (parent_node->mark == false) {
			parent_node->mark = true;
		} else {
			cut(H, parent_node,aux);
			cascading_cut(H,aux);
		}
	}
}


void decrease_key(FIB_HEAP *H, NODE *to_decrease, float new_key) {
	NODE *parent_node;
	if (H == NULL)
		return;
	if (to_decrease != NULL) {
		if (to_decrease->key >= new_key) {
			to_decrease->key = new_key;
			parent_node = to_decrease->parent;

			if ((parent_node != NULL) && (to_decrease->key < parent_node->key)) {
				cut(H,to_decrease,parent_node);
				cascading_cut(H,parent_node);
			}

			if (to_decrease->key < H->min->key) {
				H->min = to_decrease;
			}
		}

	}
}

void find_node (FIB_HEAP *H, NODE *n, float key, float new_key) {
	NODE *find_use = n;
	NODE *f = NULL;
	find_use->visited = true;
	if (find_use->key == key) {
		find_use->visited =false;
		f = find_use;
		decrease_key(H, f, new_key);
	}
	if (find_use->child != NULL) {
		find_node(H, find_use->child,key,new_key);
	}
	if ((find_use->right_sibling->visited != true)) {
		find_node(H, find_use->right_sibling, key, new_key);
	}

	find_use->visited = false;
}


void delete_node(FIB_HEAP *H, float dec_key) {
	NODE *p = NULL;
	find_node(H, H->min, dec_key, -5000);
	p = extract_min(H);
	assert(p != NULL);
}

