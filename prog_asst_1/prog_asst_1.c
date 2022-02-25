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


void delete_graph(Graph g) {
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

float prim_stats(Graph g,float* edge_sum) {
	int pred;
	float max_edge,weight;
	int num_edges = 0;
	for (int i = 0; i < g->n_v; i++) {
		pred = g->alist[i]->pi;

		if (pred != -1) {
			weight = graph_has_edge(g,pred,i);
			if (weight != 0) {
				num_edges++;
			}
			*edge_sum = *edge_sum + weight;
			if (max_edge < weight) {
				max_edge = weight;
			}
		}
	}

	if (num_edges + 1 != g->n_v) {
		printf("Non complete tree.\n");
	}

	return max_edge;
}

double cust_rand()
{
    return (double)rand() / (double)RAND_MAX ;
}

Graph initialize_full_graph(int n, float cutoff) {
	Graph full_graph = create_graph(n);

	for (int i = 0; i < n; i++) {
		for (int j = i + 1; j < n; j++) {

			float weight = cust_rand();

			if (weight < cutoff) {
				add_edge(full_graph, i,j,weight);
				add_edge(full_graph, j,i,weight);
			}

		}
	}

	return full_graph;	
}


Graph initialize_full_multi_graph(int n, float cutoff, int dimension) {

	float weight,tmp_weight;

	float* coords = malloc(sizeof(float)*n*dimension);

	assert(coords != NULL);

	for (int i = 0; i < n; i++) {
		for (int dim = 0; dim < dimension; dim++) {
			coords[n*dim + i] = cust_rand();
		}
	}

	Graph full_graph = create_graph(n);
	for (int i = 0; i < n; i++) {
		for (int j = i + 1; j < n; j++) {

			weight = 0;
			for (int dim = 0; dim < dimension; dim++) {
				tmp_weight = (coords[n*dim + i] - coords[n*dim + j]);
				weight += (tmp_weight*tmp_weight);
			}

			weight = sqrt(weight);

			if (weight < cutoff) {
				add_edge(full_graph, i,j,weight);
				add_edge(full_graph, j,i,weight);
			}
		}
	}
	return full_graph;
}

int main(int argc, char* argv[]) {

	srand(time(NULL));
	cust_rand();

	if(argc != 5) {
		printf("Usage: ./randmst cutval numpoints numtrials dimension\n");
		exit(0);
	}

	int iters = strtol(argv[3],NULL,10);
	int n_vertices = strtol(argv[2],NULL,10);
	float cutoff = strtof(argv[1],NULL);
	int dimension = strtol(argv[4],NULL,10);

	float max_overall_edge,max_tmp;
	clock_t start,end;
	Graph graph;

	float edge_sum;
	float total_edge_sum = 0;
	for(int it = 0; it < iters; it++) {

		if (dimension == 1) {
			graph = initialize_full_graph(n_vertices,cutoff);
		}
		else {
			graph = initialize_full_multi_graph(n_vertices,cutoff,dimension);
		}

		prim_shortest(graph,0);

		edge_sum = 0;
		max_tmp = prim_stats(graph,&edge_sum);
		total_edge_sum += edge_sum;
		if (max_overall_edge < max_tmp) {
			max_overall_edge = max_tmp;
		}
	}
	// This commented print was used to find the cut values; obviously don't really need it to print currently.
	//printf("Max for %d Vertices Over %d iterations: %f\n",n_vertices,iters,max_overall_edge);
	printf("Average: %f, Numpoints: %d, Numtrials: %d, Dimension: %d\n",total_edge_sum / iters, n_vertices, iters, dimension);
	printf("\n");
}


