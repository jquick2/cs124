#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

typedef struct fib_node {
	int key;
	int degree;
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
void insertion(FIB_HEAP *H, NODE *new, int val);
NODE *extract_min(FIB_HEAP *H);
void consolidate(FIB_HEAP *H);
void fib_heap_link(FIB_HEAP *H, NODE* y, NODE *x);
NODE *find_min_node(FIB_HEAP *H);
void decrease_key(FIB_HEAP *H,NODE *node, int key);
void cut(FIB_HEAP *H, NODE *node_to_be_decrease, NODE *parent_node);
void cascading_cut(FIB_HEAP *H, NODE *parent_node);
void delete_node(FIB_HEAP *H, int key);

void print_heap(NODE* n) {
	NODE *x;
	for (x = n;; x = x->right_sibling) {
		if (x->child == NULL) {
			printf("node with no child (%d) n\n",x->key);
		} else {
			printf("NODE(%d) with child (%d)n\n", x->key, x->child->key);
			print_heap(x->child);
		}
		if (x->right_sibling == n) {
			break;
		}
	}
}

FIB_HEAP *make_fib_heap(){
	
	FIB_HEAP *H = (FIB_HEAP *)malloc(sizeof(FIB_HEAP));
	H->n = 0;
	H->min = NULL;
	H->phi = 0;
	H->degree = 0;
	return H;
}

void insertion(FIB_HEAP *H, NODE *new, int val) {
	new = (NODE *)malloc(sizeof(NODE));
	new->key = val;
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
}

NODE *find_min_node(FIB_HEAP *H) {
	if (H == NULL) {
		return NULL;
	}
	else {
		return H->min;
	}
}

FIB_HEAP *unionHeap(FIB_HEAP *H1, FIB_HEAP *H2) {
	FIB_HEAP *Hnew;
	Hnew = make_fib_heap();
	Hnew->min = H1->min;

	NODE *temp1,*temp2;
	temp1 = Hnew->min->right_sibling;
	temp2 = H2->min->left_sibling;

	Hnew->min->right_sibling->left_sibling = H2->min->left_sibling;
	Hnew->min->right_sibling = H2->min;
	H2->min->left_sibling = Hnew->min;
	temp2->right_sibling = temp1;

	if ((H1->min == NULL) || (H2->min != NULL && H2->min->key < H1->min->key)) {
		Hnew->min = H2->min;
	}
	Hnew->n = H1->n + H2->n;
	return Hnew;
}

int cal_degree(int n) {
	int count = 0;
	while (n > 0) {
		n = n / 2;
		count++;
	}
	return count;
}

void consolidate(FIB_HEAP *H) {
	int degree,i,d;
	degree = cal_degree(H->n);
	NODE *A[degree], *x, *y, *z;
	for (i = 0; i <= degree; i ++) {
		A[i] = NULL;
	}
	x = H->min;
	do {
		d = x->degree;
		while (A[d] != NULL) {
			y = A[d];
			if (x->key > y->key) {
				NODE *tmp;
				tmp = x;
				x = y;
				y = tmp;
			}
			if (y == H->min)
				H->min = x;
			fib_heap_link(H,y,x);
			if (y->right_sibling == x)
				H->min = x;
			A[d] = NULL;
			d++;
		}
		A[d] = x;
		x = x->right_sibling;
	} while (x != H->min);

	H->min = NULL;
	for (i = 0; i < degree; i++) {
		if (A[i] != NULL) {
			A[i]->left_sibling = A[i];
			A[i]->right_sibling = A[i];
			if (H->min == NULL) {
				H->min = A[i];
			} else {
				H->min->left_sibling->right_sibling = A[i];
				A[i]->right_sibling = H->min;
				A[i]->left_sibling = H->min->left_sibling;
				H->min->left_sibling = A[i];
				if (A[i]->key < H->min->key) {
					H->min = A[i];
				}
			}
			if (H->min == NULL) {
				H->min = A[i];
			} else if (A[i]->key < H->min->key) {
				H->min = A[i];
			}
		}
	}
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


void decrease_key(FIB_HEAP *H, NODE *to_decrease, int new_key) {
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

void find_node (FIB_HEAP *H, NODE *n, int key, int new_key) {
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

FIB_HEAP *insertion_procedure(int no_of_nodes){
	FIB_HEAP *temp;
	int ele, i;
	NODE *new_node;
	temp = (FIB_HEAP *)malloc(sizeof(FIB_HEAP));
	temp = NULL;
	if (temp == NULL) {
		temp = make_fib_heap();
	}
	for (i = 1; i <= no_of_nodes; i++) {
		insertion(temp, new_node, i);
	}
	return temp;
}

void delete_node(FIB_HEAP *H, int dec_key) {
	NODE *p = NULL;
	find_node(H, H->min, dec_key, -5000);
	p = extract_min(H);
	assert(p != NULL);
}

