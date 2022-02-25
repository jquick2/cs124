typedef struct graph *Graph;
typedef struct adj_list_node adj_list_node;

Graph create_graph(int n);
void delete_graph(Graph);
void add_edge(Graph, int node_start, int node_end, float weight);
float graph_has_edge(Graph, int node_start, int node_end);

