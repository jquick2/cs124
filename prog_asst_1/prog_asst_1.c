#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <assert.h>

#include "graph.h"


struct graph {
	int n_v;
	int n_e;
	struct adj_list {
		int d;
		int len;
		char is_sorted;
		int list[1];
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
	}

	return g;
}


void destroy_graph(Graph g) {
	for (int i = 0; i < g->n_v; i++) {
		free(g->alist[i]);
	}
	free(g);
}


void add_edge(Graph g, int node_start, int node_end) {
	assert(node_start >= 0);
	assert(node_start < g->n_v);
	assert(node_end >= 0);
	assert(node_end < g->n_v);

	while(g->alist[node_start]->d >= g->alist[node_start]->len) {
		g->alist[node_start]->len *= 2;
		g->alist[node_start] = realloc(g->alist[node_start],sizeof(struct adj_list) + 
			sizeof(int) * (g->alist[node_start]->len -1));
	}

	g->alist[node_start]->list[g->alist[node_start]->d++] = node_end;
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

int graph_has_edge(Graph g,int node_start,int node_end) {
	assert(node_start >= 0);
	assert(node_start < g->n_v);
	assert(node_end >= 0);
	assert(node_end < g->n_v);


	for (int i = 0; i < g->alist[node_start]->d; i++) {
		if (g->alist[node_start]->list[i] == node_end) return 1;
	}
	return 0;
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


int main() {
	srand(time(NULL));
	cust_rand();

	int test = rand_gen(1);
	printf("%d\n",test);
}