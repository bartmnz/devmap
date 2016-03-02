
#include "llist.h"

#include <stdio.h>
#include <stdlib.h>


struct llist *ll_create( void *data)
{
	struct llist *head = malloc(sizeof(*head));
	if(head) {
		head->data = data;
		head->next = NULL;
	}
	return head;
}

void ll_disassemble(struct llist *l)
{
	while(l) {
		struct llist *tmp = l->next;
		free(l);
		l = tmp;
	}
}

void ll_destroy(struct llist *l)
{
	while(l) {
		struct llist *tmp = l->next;
		free((void*)l->data);
		free(l);
		l = tmp;
	}
}

void ll_add(struct llist **l,  void *data)
{
	struct llist *item = ll_create(data);
	if(item) {
		item->next = *l;
		*l = item;
	}
}

 void *ll_remove(struct llist **l)
{
	struct llist *old_head = *l;
	*l = old_head->next;
	 void *data = old_head->data;
	free(old_head);
	return data;
}

void ll_append(struct llist *a,
		struct llist *b)
{
	while(a->next) {
		a = a->next;
	}

	a->next = b;
}

bool ll_is_circular(struct llist *l)
{
	struct llist *tmp = l;
	while(l && l->next != tmp) {
		l = l->next;
	}

	if(l == NULL) {
		return false;
	} else {
		return true;
	}
}

void ll_reverse(struct llist **l)
{
	struct llist *head = *l;
	struct llist *prev = NULL;
	while(head) {
		struct llist *after = head->next;
		head->next = prev;
		prev = head;
		head = after;
	}

	*l = prev;
}

bool ll_is_sorted(struct llist *l,
		int (*cmp)( void *,  void *))
{
	while(l && l->next) {
		if(1 == cmp(l->data,
					l->next->data)) {
			return false;
		}
		l = l->next;
	}

	return true;
}

struct llist* ll_clone(struct llist *l){
	if ( ! l ){
		return NULL;
	}
	struct llist* list = ll_create(l->data);
	l = l->next;
	while( l ) {
			ll_add(&list, l->data);
			l = l->next;
	}
	return list;
}

struct llist* ll_diff(struct llist *l1, struct llist *l2, bool cmp( void* first,  void* second)){
	if ( ! l1 || !l2 ){
		return false;
	}
	struct llist* rValue = NULL;// = ll_create(NULL);
	bool inThere;
	while(l1){
		inThere = false;
		while(l2){
		//	printf("1 ");
			if (cmp(l1->data, l2->data)){
		//		printf("have match");
				inThere = true;
				break;
			}
			l2 = l2->next;
		}
		//printf("\n");
		if(!inThere){
			ll_add(&rValue, l1->data);
		}
		l1 = l1->next;
	}
	return rValue;
}

int ll_length(struct llist *l ){
	if (!l ){
		return 0;
	}
	struct llist* head = l;
	int count =0;
	while (head ){
		count++;
		head = head->next;
	}
	return count;
}