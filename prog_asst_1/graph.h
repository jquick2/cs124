typedef struct graph *Graph;

Graph create_graph(int n);

void delete_graph(Graph);

void add_edge(Graph, int node_start, int node_end);
int count_vertices(Graph);
int count_edges(Graph);

int graph_out_degree(Graph, int node);
int graph_has_edge(Graph, int node_start, int node_end);

