#include <stdbool.h>

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
NODE *insertion(FIB_HEAP *H, NODE *new, float val, int vert_id);
NODE *extract_min(FIB_HEAP *H);
void consolidate(FIB_HEAP *H);
void fib_heap_link(FIB_HEAP *H, NODE* y, NODE *x);
NODE *find_min_node(FIB_HEAP *H);
void decrease_key(FIB_HEAP *H,NODE *node, float key);
void cut(FIB_HEAP *H, NODE *node_to_be_decrease, NODE *parent_node);
void cascading_cut(FIB_HEAP *H, NODE *parent_node);
void delete_node(FIB_HEAP *H, float key);

void print_heap(NODE* n);
FIB_HEAP *insertion_procedure(int no_of_nodes);