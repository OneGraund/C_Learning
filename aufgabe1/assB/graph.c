#include "header.h"
#include <stdio.h>
#include <stdlib.h> 
#include <stdbool.h>
#include <string.h>
#include <math.h>

void construct_edges_from_str(char *in_str,
		enum connection edges[MAX_GRAPH_SIZE][MAX_GRAPH_SIZE]) {
	if (strlen(in_str) < 3) {
		debug("String is too small to contain single edge. Size: %d", (int) strlen(in_str));
	}


	int size = 0;
	int end_id;
	char node1[(int) log10(MAX_GRAPH_SIZE)+2];
	int node1_num;

	char node2[(int) log10(MAX_GRAPH_SIZE)+2];
	int node2_num;

	char *unique_nodes[MAX_GRAPH_SIZE] = {NULL};
	void add_node_if_unique(char *node_to_add) {
		int pos = (int) strtol(node_to_add, NULL, 10);
		if (unique_nodes[pos] == NULL) {
			size++;
			unique_nodes[pos] = malloc(strlen(node_to_add) + 1);
			if (unique_nodes[pos] != NULL)
				strcpy(unique_nodes[pos], node_to_add);
			else
				debug("Memory allocation failed for unique_nodes[%d]", pos);
		}
	}

	void free_unique_nodes() {
		for (int i = 0; i < MAX_GRAPH_SIZE; i++)
			free(unique_nodes[i]);
	}

	int space;
	int i = 0;

/*	char *str = malloc((int) strlen(str) + 2);
	if (str == NULL) {
		debug("Memory allocation for input string with ' ' in the beginning failed");
		free(str);
		free_unique_nodes();
		return;
	}
	str[0] = ' ';
	strcpy(str + 1, in_str);
	str[strlen(in_str)+1] = '\0';  */

	char str[strlen(in_str) + 2];
	str[0] = ' ';
	for (int i = 0; i < strlen(in_str); i++)
		str[i+1] = in_str[i];
	str[strlen(in_str)+1] = '\0';


	while (str[i] != '\0') {
		if (str[i] == ' ') {

			int def = i;
			for ( ; (def-i) < sizeof(node1); def++) {
				if (str[def] == '-')
					break;
			}

			strncpy(node1, str + (i+1), (def-i)-1);
			node1[def-i-1] = '\0';
			if (is_string_numeric(node1)) {
				node1_num = (int) strtol(node1, NULL, 10);
				add_node_if_unique(node1);
			} else {
				debug("First of nodes specified is not numeric! Node: %s", node1);
				free_unique_nodes();
				//free(str);
				return;
			}

			for (space = def; (space - def) < sizeof(node2); space++) {
				if (str[space] == ' ' || str[space] == '\0')
					break;
			}

			strncpy(node2, str + (def+1), (space-def)-1);
			node2[space-def-1] = '\0';
			if (is_string_numeric(node2)) {
				node2_num = (int) strtol(node2, NULL, 10);
				add_node_if_unique(node2);
			} else {
				debug("Second of nodes specified is not numeric! Node: %s", node2);
				free_unique_nodes();
				//free(str);
				return;
			}

			edges[node1_num][node2_num] = CONNECTED; edges[node2_num][node1_num] = CONNECTED;
		}
		i++;
	}


	for (int i = 0; i < size; i++) {
		for (int j = 0; j < size; j++) {
			if (edges[i][j] != CONNECTED && edges[i][j] == UNDEFINED)
				edges[i][j] = NOT_CONNECTED;
		}
	}
	
	//free(str);
	free_unique_nodes();
}

void create_graph(Graph *graph, int size, enum connection edges[MAX_GRAPH_SIZE][MAX_GRAPH_SIZE]) {
	if (size >= MAX_GRAPH_SIZE) {
		debug("Graph that you want to create exceeds max graph size by %d", \
			(size - MAX_GRAPH_SIZE + 1));
		return;
	}
	memcpy(graph->edges, edges, size * size * sizeof(enum connection));
	for (int i = 0; i < size; i++)
		(graph->colors)[i] = NO_COLOR;
	graph->size = size;
}

bool nodes_connected(const Graph *graph, int node1, int node2) { // const is a good practice if graph isn't changed
	if (graph == NULL) {
		debug("Input graph is NULL, exiting");
		return false;
	}

	int size = graph->size;
	if (node1 < 0 || node2 < 0 || node1 > size-1 || node2 > size-1) {
		debug("Bad graph inputed. Graph size: %d, node1: %d, node2: %d", \
			graph->size, node1, node2);
		return false;
	}

	if ((graph->edges)[node1][node2] == CONNECTED && (graph->edges)[node1][node2] == CONNECTED)
		return true;
	else
		return false;
}

void assign_color_to_node(Graph *graph, int node, enum coloring color) {
	if (graph == NULL) {
		debug("Null pointer received");
		return;
	} else if (node >= graph->size - 1) {
		debug("Node to color exceeds graph size");
		return;
	} else if ((graph->colors)[node] == RED || (graph->colors)[node] == GREEN \
			|| (graph->colors)[node] == BLUE) {
		debug("Node to color is already colored");
		return;
	} else if ((graph->colors)[node] == NO_COLOR) { // sanity check
		graph->colors[node] = color;
		return;
	}

	debug("THIS SHOULD NOT BE PRINTED");
}


void get_adjacent_nodes_of_similar_color(Graph *graph, int node, int to_return[], int *found_count) {
	if (found_count != NULL) *found_count = 0;
	else {
		debug("found_count is a null pointer");
		return;
	}

	if (graph->colors[node] == NO_COLOR) {
		debug("A node that was speicified has NO COLOR!");
		return;
	}

	for (int i = 0; i < graph->size; i++) {
		if (graph->edges[node][i] == CONNECTED && graph->colors[node] == graph->colors[i]) {
			to_return[*found_count] = i;
			(*found_count++);
		}
	}
}


#ifdef TEST
void test_graph(char *str) {
	printf("Input graph: %s\n", str);

	enum connection edges[MAX_GRAPH_SIZE][MAX_GRAPH_SIZE];
	construct_edges_from_str(str, edges);

	int size = 0;
	while (size < MAX_GRAPH_SIZE) {
		if (edges[0][size] == UNDEFINED)
			break;
		size++;
	}

	debug("Matrix representation of edges:");
	for (int i = 0; i < size + 1; i++) {
		if (i == 0)
			printf("  ");
		else
			printf("%d ", i-1);
	} printf("\n");

	for (int i = 0; i < size; i++) {
		printf("%d ", i);
		for (int j = 0; j < size; j++) {
			if (edges[i][j] == 1)
				printf("O ");
			else if (edges[i][j] == 2)
				printf("X ");
		} printf("\n");
	}

	debug("Creating graph from edges provided by str %s", str);
	Graph graph;
	create_graph(&graph, size, edges);

	bool is_connected = nodes_connected(&graph, 5, 7);
	debug("Nodes 5-7 connected? Result: %d", is_connected);
	is_connected = nodes_connected(&graph, 0, 1);
	debug("Nodes 0-1 connected? Result: %d", is_connected);

	debug("Assigning color to node 1, 0, 3");
	assign_color_to_node(&graph, 0, RED);
	assign_color_to_node(&graph, 1, RED);
	assign_color_to_node(&graph, 3, RED);
	debug("Node colors:");
	printf("\t");
	for (int i = 0; i < graph.size; i++) {
		printf("n%d-%d ", i, (graph.colors)[i]);
	} printf("\n");

	debug("Getting adjacent nodes of similar colors to node 0");
	int adjacent_nodes[MAX_GRAPH_SIZE - 1]; int found_amount = 0;
	get_adjacent_nodes_of_similar_color(&graph, 0, adjacent_nodes, &found_amount);
	debug("Found %d adjacent nodes of similar color:", found_amount);
	for (int i = 0; i < found_amount; i++) {
		printf("%d ", adjacent_nodes[i]);
	} printf("\n");
}

int main(void) {

	char input_str1[] = "0-1 0-2 0-3 1-2 1-3 2-3";
	char input_str2[] = "0-1 0-2 0-3 0-4 0-5 0-6 0-7 1-2 1-3 1-4 1-5 " \
		"1-6 1-7 2-3 2-4 2-5 2-6 2-7 3-4 3-5 3-6 3-7 4-5 4-6 4-7 5-6 6-7";

	debug("Testing first string");
	test_graph(input_str1);
	debug("Testing second string");
	test_graph(input_str2);

	return 0;
}

#endif
