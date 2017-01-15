#ifndef _MY402LIST_C_
#define _MY402LIST_C_

#include "my402list.h"
#include "stdlib.h"
#include "stdio.h"
#include "cs402.h"

int My402ListInit(My402List* my402list) {
	if (my402list) {
		// initially it should point to itself
		my402list->anchor.next = &(my402list->anchor);
		my402list->anchor.prev = &(my402list->anchor);
		my402list->anchor.obj = NULL;
		my402list->num_members = 0;
		return TRUE;
	}

	return FALSE;
}

int My402ListLength(My402List* my402list) {
	if (my402list == NULL) {
		fprintf(stderr, "%s\n", "the linked list is NULL");
		return -1;
	}
	return my402list->num_members;
}

int My402ListEmpty(My402List* my402list) {
	if (my402list == NULL) {
		fprintf(stderr, "%s\n", "the linked list is NULL");
		return TRUE;
	}

	if (My402ListLength(my402list) == 0) {
		return TRUE;
	} else {
		return FALSE;
	}
}

My402ListElem *My402ListFirst(My402List* my402list) {

	if (My402ListEmpty(my402list)) {
		return NULL;
	}

	return my402list->anchor.next;
}

My402ListElem *My402ListLast(My402List* my402list) {

	if (My402ListEmpty(my402list)) {
		return NULL;
	}

	return my402list->anchor.prev;

}

My402ListElem *My402ListNext(My402List* my402list, My402ListElem* my402listelem) {

	if (my402listelem == NULL ||
		(my402listelem == My402ListLast(my402list)) ) {
		return NULL;
	}
	return my402listelem->next;
}

My402ListElem *My402ListPrev(My402List* my402list, My402ListElem* my402listelem) {

	if (my402listelem == NULL ||
	 	(my402listelem == My402ListFirst(my402list)) ) {
		return NULL;
	}

	return my402listelem->prev;
}

My402ListElem *My402ListFind(My402List* my402list, void *obj) {
	My402ListElem* tempElem = My402ListFirst(my402list);
	while (tempElem != NULL) {
		if (tempElem->obj == obj) {
			return tempElem;
		}
		tempElem = My402ListNext(my402list, tempElem);
	}
	return NULL;
}

int My402ListAppend(My402List* my402list, void* obj) {
	My402ListElem *tempElem = (My402ListElem*) malloc(sizeof(My402ListElem));
	if (tempElem == NULL) {
		fprintf(stderr, "%s\n", "Unable to allocate memory (malloc)");
		return FALSE;
	}
	tempElem->obj = obj;
	tempElem->next = &(my402list->anchor);
	tempElem->prev = my402list->anchor.prev;

	my402list->anchor.prev->next = tempElem;
	my402list->anchor.prev = tempElem;

	my402list->num_members = my402list->num_members + 1;

	return TRUE;
}

int My402ListPrepend(My402List* my402list, void* obj) {
	My402ListElem *tempElem = (My402ListElem*) malloc(sizeof(My402ListElem));
	if (tempElem == NULL) {
		fprintf(stderr, "%s\n", "Unable to allocate memory (malloc)");
		return FALSE;
	}
	tempElem->obj = obj;
	tempElem->prev = &(my402list->anchor);
	tempElem->next = my402list->anchor.next;

	my402list->anchor.next->prev = tempElem;
	my402list->anchor.next = tempElem;

	my402list->num_members = my402list->num_members + 1;

	return TRUE;
}

void My402ListUnlink(My402List* my402list, My402ListElem* my402listelem) {
	my402list->num_members = my402list->num_members - 1;
	my402listelem->prev->next = my402listelem->next;
	my402listelem->next->prev = my402listelem->prev;
	free(my402listelem);
}

void My402ListUnlinkAll(My402List* my402list) {
	My402ListElem* tempElem = My402ListFirst(my402list);
	while (tempElem != NULL) {
		My402ListUnlink(my402list, tempElem);
		tempElem = My402ListFirst(my402list);
	}

}

int My402ListInsertAfter(My402List* my402list, void* obj, My402ListElem* my402listelem) {
	if (my402listelem == NULL) {
		return My402ListAppend(my402list, obj);
	}

	My402ListElem *tempElem = (My402ListElem*) malloc(sizeof(My402ListElem));
	if (tempElem == NULL) {
		fprintf(stderr, "%s\n", "Unable to allocate memory (malloc)");
		return FALSE;
	}
	tempElem->obj = obj;
	tempElem->prev = my402listelem;
	tempElem->next = my402listelem->next;
 
 	my402listelem->next->prev = tempElem;
 	my402listelem->next = tempElem;
 	my402list->num_members = my402list->num_members + 1;

 	return TRUE;
}

int My402ListInsertBefore(My402List* my402list, void* obj, My402ListElem* my402listelem) {
	if (my402listelem == NULL) {
		return My402ListPrepend(my402list, obj);
	}

	My402ListElem *tempElem = (My402ListElem*) malloc(sizeof(My402ListElem));
	if (tempElem == NULL) {
		fprintf(stderr, "%s\n", "Unable to allocate memory (malloc)");
		return FALSE;
	}
	tempElem->obj = obj;
	tempElem->next = my402listelem;
	tempElem->prev = my402listelem->prev;
 
 	my402listelem->prev->next = tempElem;
 	my402listelem->prev = tempElem;
 	my402list->num_members = my402list->num_members + 1;

 	return TRUE;
}
 	
#endif /*_MY402LIST_C_*/
