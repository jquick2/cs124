#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <assert.h>
#include <limits.h>
#include <float.h>

#include "graph.h"
#include "fib_heap.h"




struct adj_list_node {
	int out_node;
	float weight;
};


struct graph {
	int n_v;
	int n_e;
	struct adj_list {
		int d;
		int len;
		char is_sorted;

		float key;
		int pi;
		bool in_q;
		NODE* node_addr;
		// int list[1];
		adj_list_node* list[1];
	} *alist[1];
};

Graph create_graph(int n) {
	Graph g;
	int i;

	g = malloc(sizeof(struct graph) + sizeof(struct adj_list *) * (n-1));
	assert(g);

	g->n_v = n;
	g->n_e = 0;

	for (int i = 0; i < n; i++) {
		g->alist[i] = malloc(sizeof(struct adj_list));
		assert(g->alist[i]);

		g->alist[i]->d = 0;
		g->alist[i]->len = 1;
		g->alist[i]->is_sorted = 1;

		g->alist[i]->pi = -1;
		g->alist[i]->key = FLT_MAX;
		g->alist[i]->in_q = true;
		g->alist[i]->node_addr = NULL;
	}

	return g;
}


void destroy_graph(Graph g) {
	for (int i = 0; i < g->n_v; i++) {
		for (int j = 0; j < g->alist[i]->len; j++) {
			free(g->alist[i]->list[j]);
		}
		free(g->alist[i]);
	}
	free(g);
}


void add_edge(Graph g, int node_start, int node_end, float weight) {
	assert(node_start >= 0);
	assert(node_start < g->n_v);
	assert(node_end >= 0);
	assert(node_end < g->n_v);


	while(g->alist[node_start]->d >= g->alist[node_start]->len) {
		g->alist[node_start]->len *= 2;
		g->alist[node_start] = realloc(g->alist[node_start],sizeof(struct adj_list) + 
			sizeof(adj_list_node*) * (g->alist[node_start]->len -1));
	}

	//g->alist[node_start]->list[g->alist[node_start]->d++] = node_end;
	adj_list_node* new_node = malloc(sizeof(adj_list_node));
	g->alist[node_start]->list[g->alist[node_start]->d++] = new_node;

	new_node-> out_node = node_end;
	new_node-> weight = weight;
	g->alist[node_start]->is_sorted = 0;

	g->n_e++;
}

int count_vertices(Graph g) {
	return g->n_v;
}

int count_edges(Graph g) {
	return g->n_e;
}

int graph_out_degree(Graph g, int node) {
	assert(node >= 0);
	assert(node < g->n_v);
	return g->alist[node]->d;
}

float graph_has_edge(Graph g,int node_start,int node_end) {
	assert(node_start >= 0);
	assert(node_start < g->n_v);
	assert(node_end >= 0);
	assert(node_end < g->n_v);

	int tmp;
	if (node_start > node_end) {
		tmp = node_start;
		node_start = node_end;
		node_end = tmp;
	}

	for (int i = 0; i < g->alist[node_start]->d; i++) {
		if (g->alist[node_start]->list[i]->out_node == node_end) {
			return g->alist[node_start]->list[i]->weight;
		}
	}


	return 0;
}


void prim_shortest(Graph g, int node_start) {
	int min_node_id;
	int out_vert;
	bool in_q;

	assert(node_start >= 0);
	assert(node_start < g->n_v);

	// Initialize Fib Heap
	FIB_HEAP *Q = make_fib_heap();

	g->alist[node_start]->node_addr = insertion(Q,NULL,0,node_start);
	g->alist[node_start]->key = 0;

	for(int i = 0; i < g->n_v; i++) {
		if (node_start != i) {
			g->alist[i]->node_addr = insertion(Q,NULL,g->alist[i]->key,i);
		}
		g->alist[i]->in_q = true;
	}


	while(Q->min != NULL) {
		NODE* min_node = extract_min(Q);

		min_node_id = min_node->vert_id;
		g->alist[min_node_id]->in_q = false;

		//printf("Vertex id: %d\n",min_node->vert_id);

		for(int i = 0; i < g->alist[min_node_id]->d; i++) {
			out_vert = g->alist[min_node_id]->list[i]->out_node;
			in_q = g->alist[out_vert]->in_q;
			//printf("%d In Q: %d\n",out_vert,g->alist[out_vert]->in_q);
			if (in_q && g->alist[min_node_id]->list[i]->weight < g->alist[out_vert]->key) {
				// Update both fib_heap and graph storage of key
				g->alist[out_vert]->key = g->alist[min_node_id]->list[i]->weight;
				g->alist[out_vert]->pi = min_node_id;
				//printf("here2\n");
				decrease_key(Q,g->alist[out_vert]->node_addr,g->alist[out_vert]->key);
			}
		}
	}
}

float prim_stats(Graph g) {
	int pred;
	float max_edge,weight;
	for (int i = 0; i < g->n_v; i++) {
		pred = g->alist[i]->pi;

		if (pred != -1) {
			//printf("Edge in Tree: (%d,%d), weight %f\n",pred,i,graph_has_edge(g,pred,i));
			weight = graph_has_edge(g,pred,i);
			if (max_edge < weight) {
				max_edge = weight;
			}
		}
	}

	return max_edge;
}


double cust_rand()
{
    return (double)rand() / (double)RAND_MAX ;
}

void generate_n_vert_graph() {
	return;
}

// Generate n-dim array in the unit n-space
int rand_gen(int n) {

	printf("%f\n",cust_rand());
	printf("%f\n",cust_rand());
	printf("%f\n",cust_rand());


	return 0;
}


void print_graph_edge_check(Graph g, int node_start, int node_end) {
	float weight = graph_has_edge(g,node_start,node_end);
	if (weight > 0) {
		printf("Has edge (%d, %d) with weight %f\n",node_start,node_end,weight);
	}
	return;
}


Graph initialize_full_graph(int n) {
	Graph full_graph = create_graph(n);

	for (int i = 0; i < n; i++) {
		for (int j = i + 1; j < n; j++) {
			float weight = cust_rand();
			add_edge(full_graph, i,j,weight);
			add_edge(full_graph, j,i,weight);
		}
	}

	return full_graph;	
}

int main() {
	/*for (int i = 0; i < test_graph->n_v; i++) {
		for (int j = i + 1; j < test_graph->n_v; j++) {
			print_graph_edge_check(test_graph,i,j);
		}
	}*/
	srand(time(NULL));
	cust_rand();

	float max_overall_edge,max_tmp;
	int iters = 1000;
	int n_vertices = 333;
	for(int it = 0; it < iters; it++) {
		Graph graph = initialize_full_graph(n_vertices);

		prim_shortest(graph,0);
		max_tmp = prim_stats(graph);
		if (max_overall_edge < max_tmp) {
			max_overall_edge = max_tmp;
		}
	}
	printf("Max for %d Vertices Over %d iterations: %f\n",n_vertices,iters,max_overall_edge);

}


