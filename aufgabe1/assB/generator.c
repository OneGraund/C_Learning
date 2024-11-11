/**
 * @file generator.c
 *
 * @brief Randomly generate solution and write to circular buffer
 *
 * @details The generator program takes a graph as input. The program repeatedly generates a random solution
 *		    to the problem and writes its result to the circular buffer. It repeats this procedure unti it
 *		    is notified by the supervisor to terminate. The generator muyst not make any assumptions as to
 *		    the number of edges that may be specified and programmatically accept a graph of any size.
 *          The generate random sets of edges which must be removed to make the given graph 3-colorable,
 *			following algorithm is used:
 *			\n\t Generate a random (not necessarily valid) 3-coloring of the graph, i.e. randomly assign one
 *				 of 3 colors to each vertex of the graph
 *			\n\t Select all edges (u, v) for which the color u is identical to the color of v. These edges
 *				 need to be removed to make the graph 3-colorable.
 *
 * @synopsis
 *		generator EDGE1 ...
 * @param EDGE1 One edge of a graph. At least one edge must be given. An edge is specified by a string,
 *				with the indices of the two nodes it connects separated by a - .
 *
 * @code
 * # Run the generator with a sample graph containing several edges
 * ./generator 0-1 0-2 0-3 1-2 1-3 2-3
 * # The edges here connect nodes in pairs (e.g. 0-1 connects node 0 and node 1)
 * @endcode
 *
 * @author Volodymyr Skoryi
 * @date 08.11.2024
 *
 */

int main(void) {}
