#include "clist.h"
#include <stdlib.h>


static list_t *create_list(void);
static node_t *create_node(void *data);

/*
* Initialize a new list
*/
list_t *create_list(void) {
	list_t *list = new list_t;//(list_t *)malloc(sizeof(list_t));
	list->head = NULL;
	list->tail = NULL;
	list->size = 0;
	return list;
}

/*
* Initialize a new node
*/
node_t *create_node(void *data) {
	node_t *node = new node_t;//(node_t *)malloc(sizeof(node_t));
	node->prev = NULL;
	node->next = NULL;
	node->data = data;
	return node;
}

/*
* Function for creating a new list
*/
list_t *new_list(void) {
	return create_list();
}

///*
//* Push a node to the front of the list
//*/
//void push_front(list_t *list, void *data) {
//	node_t *node = create_node(data);
//
//	/* Check if the list is empty */
//	if (list->size == 0) {
//		/* In an empty list prev and next are the first node */
//		node->prev = NULL;
//		node->next = NULL;
//		list->tail = node;
//
//	}
//	else {
//		node_t *ohead = list->head;
//
//		/* Set the new nodes prev and next pointers */
//		node->next = ohead;
//		node->prev = NULL;
//
//		/* Set the previous and next pointer to the new node */
//		ohead->prev = node;
//
//	}
//
//	list->head = node;
//	++list->size;
//}

/*
* Push a node to the back of the list
*
*/
void push_back(list_t *list, void *data) {
	node_t *node = create_node(data);

	push_back_node(list, node);
}

void push_back_node(list_t *list, node_t *node)
{
	if (list->size == 0) {
		node->prev = NULL;
		node->next = NULL;

		/* Since this is the only node it will be the head by default */
		list->head = node;
		list->tail = node;
	}
	else {
		node_t *otail = list->tail;

		node->next = NULL;
		node->prev = otail;

		otail->next = node;

		list->tail = node;
	}

	++list->size;
}

/*
* Removes the first node in the list
*/
int remove_front(list_t *list, list_op free_func) {
	if (list->size == 0) {
		return -1;
	}

	node_t *head = list->head;

	if (list->size == 1) {
		list->head = NULL;
		list->tail = NULL;
	}
	else {
		node_t *next = head->next;

		list->head = next;

		next->prev = NULL;
	}

	/* Provided free function */
	free_func(head->data);
	free(head);

	--list->size;

	return 0;
}

///*
//* Remove a node at the specified index
//*/
//int remove_index(list_t *list, unsigned int index, list_op free_func) {
//	if (list->size == 0) {
//		return -1;
//	}
//
//	/* Check to make sure the is data at the index */
//	if (!list->size || index >= list->size) {
//		return NULL;
//	}
//
//	/* Loop through until you reach the requested index */
//	node_t *current_node = list->head;
//	for (unsigned int i = 0; i < index; ++i) {
//		current_node = current_node->next;
//	}
//
//	node_t *next = current_node->next;
//	node_t *prev = current_node->prev;
//
//	/* Update the pointers to remove the node */
//	if (prev)
//		prev->next = next;
//	else
//		list->head = next;
//
//	if (next)
//		next->prev = prev;
//	else
//		list->tail = prev;
//
//	free_func(current_node->data);
//	free(current_node);
//
//	--list->size;
//
//	return 0;
//}

///*
//* Remove the last node in the list
//*/
//int remove_back(list_t *list, list_op free_func) {
//	if (list->size == 0) {
//		return -1;
//	}
//
//	node_t *to_remove = list->tail; /* Node which will be removed */
//
//	if (list->size == 1) {
//		list->head = NULL;
//		list->tail = NULL;
//	}
//	else {
//		node_t *new_back = to_remove->prev; /* The new back */
//
//											/* Update to reflect the new back */
//
//		new_back->next = NULL;
//		list->tail = new_back;
//	}
//
//	free_func(to_remove->data);
//	free(to_remove);
//
//	--list->size;
//
//	return 0;
//}

void remove_node_do_not_free(list_t *list, node_t *node)
{
	node_t *prev = node->prev;
	node_t *next = node->next;

	if (prev)
		prev->next = next;
	else
		list->head = next;

	if (next)
		next->prev = prev;
	else
		list->tail = prev;

	--list->size;
}

/*
* Get data from the front of the list
*/
void *get_front(list_t *list) {
	if (list->size == 0) {
		return NULL;
	}

	return list->head->data;
}

///*
//* Get data from an index in the list
//*/
//void *get_index(list_t *list, unsigned int index) {
//	/* Check to make sure the is data at the index */
//	if (!list->size || index >= list->size) {
//		return NULL;
//	}
//
//	/* Loop through until you reach the requested index */
//	node_t *current_node = list->head;
//	for (unsigned int i = 0; i < index; ++i) {
//		current_node = current_node->next;
//	}
//
//	return current_node->data;
//}

///*
//* Get data from the back of the list
//*/
//void *get_back(list_t *list) {
//	if (list->size == 0) {
//		return NULL;
//	}
//
//	return list->tail->data;
//}

/*
* Check if the list is empty
*/
int is_empty(list_t *list) {
	return (list->size == 0);
}

/*
* Return the length of the list
*/
int get_size(list_t *list) {
	return list->size;
}

void empty_list(list_t *list, list_op free_func) {

	node_t *current_node = list->head;
	node_t *next;

	while (current_node)
	{
		next = current_node->next;

		free_func(current_node->data);
		free(current_node);
		current_node = next;
	}

	/* Reset the list to a clean state */
	list->head = NULL;
	list->tail = NULL;
	list->size = 0;
}