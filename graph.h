#ifndef GRAPH_H
 #define GRAPH_H

#include <stdbool.h>
#include <stdlib.h>

#include "llist.h"

typedef struct graph {
	struct node *nodes;
}graph;



graph *graph_create(void);
void graph_disassemble(graph *g);
void graph_destroy(graph *g);

bool graph_add_node(graph *g,  void* data);
bool graph_add_edge(graph *g,  void* from,  void* to, int weight, bool cmp( void* first,  void* second));

bool graph_has_node(graph *g,  void* data, bool cmp( void* first,  void* second));

bool graph_remove_node(graph *g,  void* data, bool cmp( void* first,  void* second));
bool graph_remove_edge(graph *g,  void* from,  void* to, bool cmp( void* first,  void* second));

size_t graph_node_count( graph *g);
size_t graph_edge_count( graph *g);

int graph_edge_weight( graph *g,  void* from,  void* to, bool cmp( void* first,  void* second));

struct llist *graph_adjacent_to( graph *g,  void* data, bool cmp( void* first,  void* second));

void graph_print( graph *g, void to_print( void *, bool is_node));

void print_edges(graph *g);

graph *graph_clone(graph *g1);

bool compare_pointers( void* first,  void* second);

struct llist *graph_list_nodes(graph *g);

#endif