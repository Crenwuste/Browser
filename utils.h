// Traistaru Dragos-Andrei 313CA

#ifndef UTILS_H
#define UTILS_H

#define MAX_SIZE 50
#define MAX_BUFF 256

typedef struct page_t page_t;
struct page_t {
	int id;
	char url[MAX_SIZE];
	char *descp;
};

typedef struct tab_t tab_t;
struct tab_t {
	int id;
	page_t *curr_page;
	struct my_stack_t *forw_stack;
	struct my_stack_t *back_stack;
};

typedef struct tabList_t tabList_t;
struct tabList_t {
	tab_t *tab_data;
	tabList_t *prev;
	tabList_t *next;
};

typedef struct browser_t browser_t;
struct browser_t {
	tab_t *curr;
	struct tabList_t *list;
};

typedef struct stack_node_t stack_node_t;
struct stack_node_t {
	page_t *data;
	struct stack_node_t *next;
};

typedef struct my_stack_t my_stack_t;
struct my_stack_t {
	struct stack_node_t *head;
};

#endif
