
#ifndef LLIST_H
 #define LLIST_H

#include <stdbool.h>

struct llist {
	 void *data;
	struct llist *next;
};

struct llist *ll_create( void *data);
void ll_disassemble(struct llist *l);
void ll_destroy(struct llist *l);

void ll_add(struct llist **l,  void *data);
 void *ll_remove(struct llist **l);


void ll_append(struct llist *a,
		struct llist *b);
bool ll_is_circular(struct llist *l);
void ll_reverse(struct llist **l);

bool ll_is_sorted(struct llist *l,
		int (*cmp)( void *,  void *));

struct llist* ll_clone(struct llist *l);

struct llist* ll_diff(struct llist *l1, struct llist *l2, bool cmp( void* first,  void* second));

int ll_length(struct llist *l );

#endif