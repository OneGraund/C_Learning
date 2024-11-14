#ifndef GRAPH_H
#define GRAPH_H

#include <stdio.h>
#include <stdbool.h>

#define MAX_GRAPH_SIZE 64

enum coloring {NO_COLOR, RED, GREEN, BLUE};
enum connection {UNDEFINED, NOT_CONNECTED, CONNECTED};

typedef struct {
	enum connection edges[MAX_GRAPH_SIZE][MAX_GRAPH_SIZE];
	enum coloring colors[MAX_GRAPH_SIZE];
	int size;
} Graph;

void construct_edges_from_str(char *str,
		enum connection edges[MAX_GRAPH_SIZE][MAX_GRAPH_SIZE]);

void create_graph(Graph *graph, int size, enum connection edges[MAX_GRAPH_SIZE][MAX_GRAPH_SIZE]);

bool nodes_connected(const Graph *graph, int node1, int node2);

void assign_color_to_node(Graph *graph, int node, enum coloring color);

void get_adjacent_nodes_of_similar_color(Graph *graph, int node, int to_return[],
	int *found_count);

void remove_edge_between(Graph *graph, int node1, int node2);

// I don't really know why, but it won't print if graph itself is sent
void print_adjacency_matrix(Graph *graph);

int define_graph_size_by_edges(enum connection edges[MAX_GRAPH_SIZE][MAX_GRAPH_SIZE]);

#endif
