
#include "graph.h"
#include "octTree.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

struct edge {
	int weight;
	struct node *to;
	struct edge *next;
};

struct node {
	 void *data;
	struct edge *edges;
	struct node *next;
};

void __graph_remove_edges_from_node(struct node *v);

graph *graph_create(void)
{
	graph *g = malloc(sizeof(*g));
	memset(g, 0, sizeof(*g));
	if(!g) {
		return NULL;
	}
	g->nodes = NULL;

	return g;
}

void graph_disassemble(graph *g)
{
	if(!g) {
		return;
	}

	struct node *curr = g->nodes;
	while(curr) {
		struct edge *e = curr->edges;
		while(e) {
			struct edge *tmp = e->next;
			free(e);

			e = tmp;
		}

		struct node *tmp = curr->next;
		free(curr);

		curr = tmp;
	}

	free(g);
}

void graph_destroy(graph *g)
{
	if(!g) {
		return;
	}

	struct node *curr = g->nodes;
	while(curr) {
		struct edge *e = curr->edges;
		while(e) {
			struct edge *tmp = e->next;
			free(e);

			e = tmp;
		}

		struct node *tmp = curr->next;
		free((void*)curr->data);
		free(curr);

		curr = tmp;
	}

	free(g);
}

bool graph_add_node(graph *g,  void *data, bool cmp(void*,void*))
{
	if(!g) {
		return false;
	}
	struct node* head = g->nodes;
	while (head){
		if ( cmp(data, head->data)){
			fprintf(stdout, "ERROR: already in graph \n");
			return false;
		}
		head = head->next;
	}
	struct node *new_node = malloc(sizeof(*new_node));
	memset(new_node, 0, sizeof(*new_node));
	if(!new_node) {
		return false;
	}
	new_node->data = data;
	new_node->edges = NULL;
	new_node->next = g->nodes;

	g->nodes = new_node;

	return true;
}

bool graph_add_edge(graph *g,  void *from,  void *to, int weight, bool cmp( void* first,  void* second))
{
	if(!g) {
		return false;
	}
	struct node *node_from=NULL, *node_to=NULL;

	// Find the two nodes; it is allowed for them to be the same node
	struct node *curr = g->nodes;
	while(curr && !(node_from && node_to)) {
		if(cmp(curr->data, from)) {
			node_from = curr;
		}
		if(cmp(curr->data, to)) {
			node_to = curr;
		}
		curr = curr->next;
	}
	if(!(node_from && node_to)) {
		printf("node_from  %s graph\n", node_from ? "in" : " not in");
		printf("node_to    %s graph\n", node_to ? "in" : " not in");
		return false;
	}

	// Check if an edge already exists between the two
	struct edge *check = node_from->edges;
	while(check) {
		if(check->to == node_to) {
			check->weight = weight;
			return true;
		}
		check = check->next;
	}

	// Insert new edge at front of list
	struct edge *new_edge = malloc(sizeof(*new_edge));
	memset(new_edge, 0, sizeof(*new_edge));
	if(!new_edge) {
		return false;
	}
	new_edge->weight = weight;
	new_edge->to = node_to;
	new_edge->next = node_from->edges;

	node_from->edges = new_edge;

	return true;
}

bool graph_has_node(graph *g,  void *data, bool cmp( void* first,  void* second))
{
	if(!g) {
		return false;
	}

	struct node *curr = g->nodes;
	while(curr) {
		if(cmp(curr->data, data)) {
			return true;
		}

		curr = curr->next;
	}

	return false;
}

bool graph_remove_node(graph *g, void *data, bool cmp( void* first,  void* second))
{
	if(!g) {
		return false;
	}

	bool removed = false;
	struct node *curr_node = g->nodes;
	struct node *prev_node = curr_node;
	struct node *target = NULL;

	if(!curr_node) {
		return removed;
	} else if(cmp(curr_node->data, data)) {
		g->nodes = curr_node->next;

		__graph_remove_edges_from_node(curr_node);

		target = curr_node;
		free(curr_node);
		curr_node = g->nodes;
		removed = true;
	}

	while(curr_node) {
		if(cmp(curr_node->data, data)) {
			prev_node->next = curr_node->next;

			__graph_remove_edges_from_node(curr_node);

			target = curr_node;
			free(curr_node);
			curr_node = prev_node->next;
			removed = true;
			continue;
		}

		struct edge *curr_edge = curr_node->edges;
		struct edge *prev_edge = curr_edge;

		if(!curr_edge) {
			goto NEXT_NODE;
		} else if((target && curr_edge->to == target) || cmp(curr_edge->to->data, data)) {
			curr_node->edges = curr_edge->next;
			free(curr_edge);
			goto NEXT_NODE;
		}

		while(curr_edge) {
			if((target && curr_edge->to == target) || cmp(curr_edge->to->data, data)) {
				prev_edge->next = curr_edge->next;
				free(curr_edge);
				break;
			}

			prev_edge = curr_edge;
			curr_edge = curr_edge->next;
		}

NEXT_NODE:
		prev_node = curr_node;
		curr_node = curr_node->next;
	}

	return removed;
}

bool graph_remove_edge(graph *g,  void *from,  void *to, bool cmp( void* first,  void* second))
{
	if(!g) {
		return false;
	}

	struct node *curr = g->nodes;
	while(curr) {
		if(!cmp(curr->data, from)) {
			curr = curr->next;
			continue;
		}

		struct edge *check = curr->edges;
		if(!check) {
			return false;
		} else if(cmp(check->to->data, to)) {
			curr->edges = check->next;
		//	printf("removed\n\n");
			free(check);
			return true;
		}

		while(check && check->next) {
			if(cmp(check->next->to->data, to)) {
				struct edge *to_remove = check->next;
				check->next = to_remove->next;
				free(to_remove);
			//	printf("removed\n\n");
				return true;
			}

			check = check->next;
		}

		return false;
	}

	return false;
}

size_t graph_node_count( graph *g)
{
	if(!g) {
		return 0;
	}

	size_t count = 0;
	struct node *curr = g->nodes;
	while(curr) {
		++count;
		curr = curr->next;
	}

	return count;
}

size_t graph_edge_count( graph *g)
{
	if(!g) {
		return 0;
	}

	size_t count = 0;
	struct node *curr = g->nodes;
	while(curr) {
		struct edge *check = curr->edges;
		while(check) {
			++count;
			check = check->next;
		}
		curr = curr->next;
	}

	return count;
}

int graph_edge_weight( graph *g,  void *from,  void *to, bool cmp( void* first,  void* second))
{
	if(!g) {
		return 0;
	}

	struct node *curr = g->nodes;
	while(curr && !cmp(curr->data, from)) {
		curr = curr->next;
	}
	if(!curr) {
		return 0;
	}

	struct edge *check = curr->edges;
	while(check && cmp(check->to->data, to)) {
			check = check->next;
	}
	if(!check) {
		return 0;
	}

	return check->weight;
}

struct llist *graph_adjacent_to( graph *g,  void *data, bool cmp( void* first,  void* second))
{
	if(!g || !data) {
		return NULL;
	}

	struct node *curr = g->nodes;
	while(curr) {
		if(cmp(curr->data, data)) {
			if(!curr->edges) {
				return NULL;
			}

			struct llist *head = NULL;
			struct edge *adj = curr->edges;
			while(adj) {
				ll_add(&head, adj->to->data);
				adj = adj->next;
			}

			return head;
		}
		curr = curr->next;
	}

	return NULL;
}

void graph_print( graph *g, void to_print( void *, bool is_node))
{
	if(!g) {
		return;
	}

	struct node *curr = g->nodes;
	while(curr) {
		to_print(curr->data, true);

		struct edge *check = curr->edges;
		while(check) {
			to_print(check->to->data, false);
			check = check->next;
		}

		curr = curr->next;
	}
}

void __graph_remove_edges_from_node(struct node *v)
{
	struct edge *e = v->edges;
	while(e) {
		struct edge *tmp = e->next;
		free(e);

		e = tmp;
	}
}

void print_edges(graph *g){
    struct node* node = g->nodes;
    while( node ){
    	struct device* device = (struct device* ) node->data;
        fprintf(stdout, "(%d) is connected to: ", device->device_id);
        struct edge* temp = node->edges;
        while( temp ){
        	device = (struct device* ) temp->to->data;
            fprintf(stdout, "(%d) ", device->device_id);
            temp = temp->next;
        }
        printf("\n");
    node= node->next;
    }
}

graph *graph_clone(graph *g1){
	if( ! g1 ){
		return NULL;
	}
	graph *g2 = graph_create();
	struct node* nodes = g1->nodes;
	while (nodes){
		graph_add_node(g2, nodes->data, compare_pointers);
		nodes = nodes->next;
	}
	nodes = g1->nodes;
	
	while ( nodes ){
	//	printf("adding edges\n");
		struct edge* edges = nodes->edges;
		while(edges){
			graph_add_edge(g2, nodes->data, edges->to->data, edges->weight, compare_pointers);
			edges = edges->next;
		}
		nodes = nodes->next;
	}
	return g2;
}

bool compare_pointers( void* first,  void* second){
    if (! first || ! second ){
        return false;
    }
    if(first == second){
    }
    return first == second;
}

struct llist *graph_list_nodes(graph *g){
	if (! g) return NULL;
	struct llist* rValue = NULL; // = ll_create(NULL);
	struct node* nodes = g->nodes;
	while( nodes){
		ll_add(&rValue, nodes->data);
		nodes = nodes->next;
	}
	return rValue;
}
