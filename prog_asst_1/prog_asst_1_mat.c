#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <assert.h>

#include "graph_mat_rep.h"


struct graph {
	int n_v;
	int n_e;
	float* adj_matrix;
};

Graph create_graph(int n) {
	Graph g;

	g = malloc(sizeof(struct graph));
	assert(g);

	g->n_v = n;
	g->n_e = 0;
	g->adj_matrix = malloc(sizeof(float) * n *n);
	return g;
}


void destroy_graph(Graph g) {
	free(g->adj_matrix);
	free(g);
}


void add_edge(Graph g, int node_start, int node_end, float weight) {
	assert(node_start >= 0);
	assert(node_start < g->n_v);
	assert(node_end >= 0);
	assert(node_end < g->n_v);

	if (node_end > node_start) {
		*((g->adj_matrix + node_start*(g->n_v)) + node_end) = weight;
	}
	else if (node_start < node_end) {
		*((g->adj_matrix + node_end*(g->n_v)) + node_start) = weight;
	}
	else {
		return;
	}
	
	g->n_e++;
}

int count_vertices(Graph g) {
	return g->n_v;
}

int count_edges(Graph g) {
	return g->n_e;
}

/*int graph_out_degree(Graph g, int node) {
	assert(node >= 0);
	assert(node < g->n_v);
	return g->alist[node]->d;
}*/

float graph_has_edge(Graph g,int node_start,int node_end) {
	assert(node_start >= 0);
	assert(node_start < g->n_v);
	assert(node_end >= 0);
	assert(node_end < g->n_v);

	float weight = 0;
	if (node_start < node_end) {
		weight = *((g->adj_matrix + node_start*(g->n_v)) + node_end);
	}
	else if (node_end < node_start) {
		weight = *((g->adj_matrix + node_end*(g->n_v)) + node_start);
	}

	return weight;
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
		//printf("Has edge (%d, %d) with weight %f\n",node_start,node_end,weight);
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

	/*int test = rand_gen(1);
	printf("%d\n",test);*/

	Graph test_graph = initialize_full_graph(20000);

	printf("done\n");

	for (int i = 0; i < test_graph->n_v; i++) {
		for (int j = 0; j < test_graph->n_v; j++) {
			print_graph_edge_check(test_graph,i,j);
		}
	}
}