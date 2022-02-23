#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <assert.h>
#include <limits.h>

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

		int key;
		int pi;
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

		g->alist[i]->key = INT_MAX;
		g->alist[i]->pi = -1;
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
	assert(node_start >= 0);
	assert(node_start < g->n_v);

	g->alist[node_start] = 0;
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
			add_edge(full_graph, i,j,cust_rand());
		}
	}

	return full_graph;	
}

int main() {
	srand(time(NULL));
	cust_rand();

	FIB_HEAP *H = insertion_procedure(5);
	print_heap(H->min);

	/*int test = rand_gen(1);
	printf("%d\n",test);*/

	//Graph test_graph = initialize_full_graph(10);

	/*for (int i = 0; i < test_graph->n_v; i++) {
		for (int j = 0; j < test_graph->n_v; j++) {
			print_graph_edge_check(test_graph,i,j);
		}
	}*/

	//prim_shortest(test_graph,0);
}