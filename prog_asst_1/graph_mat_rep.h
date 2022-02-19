typedef struct graph *Graph;
typedef struct adj_list_node adj_list_node;

Graph create_graph(int n);

void delete_graph(Graph);

void add_edge(Graph, int node_start, int node_end, float weight);
int count_vertices(Graph);
int count_edges(Graph);

int graph_out_degree(Graph, int node);
float graph_has_edge(Graph, int node_start, int node_end);

