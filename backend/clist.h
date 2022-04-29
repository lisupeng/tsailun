#ifndef _CLIST_H
#define _CLIST_H

/*
* The node structure.
*
* prev = The previous node in the list
* next = The next node in the list
* data = The data
*/
typedef struct node {
	struct node *prev;
	struct node *next;
	void *data;
} node_t;

/*
* The list struct
*
* head = Points to the head of the list
* size = Size of the list
*/
typedef struct list {
	node_t *head;
	node_t *tail;
	unsigned int size;
} list_t;

/* Void function */
typedef void(*list_op)(void*);

/* Functions */
list_t *new_list(void);
//void push_front(list_t *list, void *data);
void push_back(list_t *list, void *data);
void push_back_node(list_t *list, node_t *node);

int remove_front(list_t *list, list_op free_func);
//int remove_index(list_t *list, unsigned int index, list_op free_func); // uncomment after UT & code review
//int remove_back(list_t *list, list_op free_func);

void remove_node_do_not_free(list_t *list, node_t *node);

void *get_front(list_t *list);
//void *get_index(list_t *list, unsigned int index);
//void *get_back(list_t *list);
int is_empty(list_t *list);
int get_size(list_t *list);
void empty_list(list_t *list, list_op free_func);


#endif // _CLIST_H