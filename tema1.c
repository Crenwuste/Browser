// Traistaru Dragos-Andrei 313CA

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "utils.h"

//Functia elimina toate nodurile din stiva
void clear_stack(my_stack_t *st)
{
	if (!st)
		return;

	stack_node_t *current = st->head;
	while (current) {
		stack_node_t *temp = current;
		current = current->next;

		if (temp->data) {
			temp->data = NULL;
		}

		free(temp);
	}

	st->head = NULL;
}

void free_stack(my_stack_t *st)
{
	if (!st)
		return;

	stack_node_t *curr = st->head;
	while (curr) {
		stack_node_t *next = curr->next;
		curr->data = NULL;
		free(curr);

		curr = next;
	}

	free(st);
}

browser_t *create_browser(void)
{
	browser_t *browser = malloc(sizeof(browser_t));
	if (!browser) {
		fprintf(stderr, "Memory allocation failed\n");
		return NULL;
	}

	// Creăm santinela listei de tab-uri
	browser->list = malloc(sizeof(tabList_t));
	if (!browser->list) {
		fprintf(stderr, "Memory allocation failed\n");
		return NULL;
	}

	browser->list->tab_data = NULL;
	browser->list->prev = browser->list; // Circular
	browser->list->next = browser->list;

	browser->curr = NULL;

	return browser;
}

void add_tab(browser_t *browser, page_t *new_page, int id)
{
	if (!browser || !new_page)
		return;

	tab_t *new_tab = malloc(sizeof(tab_t));
	if (!new_tab) {
		fprintf(stderr, "Memory allocation failed\n");
		return;
	}

	new_tab->back_stack = NULL;
	new_tab->forw_stack = NULL;
	new_tab->id = id;
	new_tab->curr_page = new_page;

	tabList_t *new_node = malloc(sizeof(tabList_t));
	if (!new_node) {
		fprintf(stderr, "Memory allocation failed\n");
		return;
	}

	new_node->tab_data = new_tab;

	tabList_t *sentinel = browser->list;
	tabList_t *last = sentinel->prev;
	new_node->next = sentinel;
	new_node->prev = last;
	last->next = new_node;
	sentinel->prev = new_node;

	// Facem ca acest tab să fie curent
	browser->curr = new_tab;
}

void open_tab(FILE *out, browser_t *browser, int id)
{
	if (!browser)
		return;

	tabList_t *temp = browser->list->next;

	while (temp != browser->list) {
		if (temp->tab_data->id == id) {
			browser->curr = temp->tab_data;
			return;
		}
		temp = temp->next;
	}
	fprintf(out, "403 Forbidden\n");
}

void close_tab(FILE *out, browser_t *browser)
{
	if (!browser)
		return;

	if (browser->curr->id == 0) {
		fprintf(out, "403 Forbidden\n");
		return;
	}

	tabList_t *temp = browser->list->next;
	while (temp != browser->list) {
		if (temp->tab_data == browser->curr) {
			browser->curr = temp->prev->tab_data;
			temp->next->prev = temp->prev;
			temp->prev->next = temp->next;

			// Golim stivele
			if (temp->tab_data->back_stack) {
				free_stack(temp->tab_data->back_stack);
			}
			if (temp->tab_data->forw_stack) {
				free_stack(temp->tab_data->forw_stack);
			}

			free(temp->tab_data);
			free(temp);
			return;
		}
		temp = temp->next;
	}
	fprintf(out, "403 Forbidden\n");
}

void next_tab(FILE *out, browser_t *browser)
{
	tabList_t *temp = browser->list->next;

	while (temp != browser->list) {
		if (temp->tab_data == browser->curr) {
			if (temp->next == browser->list)
				temp = temp->next;

			browser->curr = temp->next->tab_data;
			return;
		}
		temp = temp->next;
	}
	fprintf(out, "403 Forbidden\n");
}

void prev_tab(FILE *out, browser_t *browser)
{
	tabList_t *temp = browser->list->next;

	while (temp != browser->list) {
		if (temp->tab_data == browser->curr) {
			if (temp->prev == browser->list)
				temp = temp->prev;

			browser->curr = temp->prev->tab_data;
			return;
		}
		temp = temp->next;
	}
	fprintf(out, "403 Forbidden\n");
}

my_stack_t *create_stack(void)
{
	my_stack_t *st = malloc(sizeof(my_stack_t));
	if (!st) {
		fprintf(stderr, "Malloc failed\n");
		return NULL;
	}

	st->head = NULL;
	return st;
}

void push(my_stack_t *st, page_t *page)
{
	stack_node_t *node = malloc(sizeof(stack_node_t));
	if (!node) {
		fprintf(stderr, "Malloc failed\n");
		return;
	}

	node->data = page;
	node->next = st->head;
	st->head = node;
}

page_t *pop(my_stack_t *st)
{
	if (!st || !st->head)
		return NULL;

	stack_node_t *temp = st->head;
	page_t *page = temp->data;

	st->head = st->head->next;

	free(temp);
	return page;
}

void swap_page(FILE *out, browser_t *browser, page_t *page, int page_id, int n)
{
	int prev_id, new_id, ok = 0;
	for (int i = 0; i <= n; i++) {
		if (page[i].id == page_id) {
			new_id = i;
			ok = 1;
		}
		if (page[i].id == browser->curr->curr_page->id) {
			prev_id = i;
		}
	}

	if (ok == 0) {
		fprintf(out, "403 Forbidden\n");
		return;
	}

	browser->curr->curr_page = &page[new_id]; // Schimbam pagina
	if (!browser->curr->back_stack)
		browser->curr->back_stack = create_stack();
	push(browser->curr->back_stack, &page[prev_id]);
	clear_stack(browser->curr->forw_stack);
}

void back_page(FILE *out, tab_t *tab)
{
	// Daca n-avem la ce sa ne intoarcem
	if (!tab || !tab->back_stack || !tab->back_stack->head) {
		fprintf(out, "403 Forbidden\n");
		return;
	}

	if (!tab->forw_stack)
		tab->forw_stack = create_stack();

	push(tab->forw_stack, tab->curr_page);

	tab->curr_page = pop(tab->back_stack);
}

void next_page(FILE *out, tab_t *tab)
{
	// Daca n-avem la ce sa inaintam
	if (!tab || !tab->forw_stack || !tab->forw_stack->head) {
		fprintf(out, "403 Forbidden\n");
		return;
	}

	if (!tab->back_stack)
		tab->back_stack = create_stack();

	push(tab->back_stack, tab->curr_page);

	tab->curr_page = pop(tab->forw_stack);
}

void f_print(FILE *out, browser_t *browser)
{
	tabList_t *temp = browser->list->next;

	while (temp->tab_data != browser->curr) { // Parcurgem pana la tab-ul curent
		temp = temp->next;
	}

	fprintf(out, "%d", temp->tab_data->id); // Id-ul tabu-ului curent
	temp = temp->next;
	while (temp->tab_data != browser->curr) {
		if (temp->tab_data != browser->list->tab_data)
			fprintf(out, " %d", temp->tab_data->id);
		temp = temp->next;
	}

	fprintf(out, "\n%s\n", browser->curr->curr_page->descp);
}

// Functia printeaza stiva forward de la primul element adaugat la ultimul
void print_forw_stack(FILE *out, stack_node_t *node)
{
	if (!node)
		return;

	print_forw_stack(out, node->next);
	fprintf(out, "%s\n", node->data->url);
}

void history_print(FILE *out, browser_t *browser, int tab_id)
{
	int ok = 0;
	tabList_t *temp = browser->list->next;

	while (temp != browser->list) {
		if (temp->tab_data->id == tab_id) { // Verific daca exista tab-ul
			ok = 1;
			break;
		}
		temp = temp->next;
	}

	if (!ok) {
		fprintf(out, "403 Forbidden\n");
		return;
	}

	if (temp->tab_data->forw_stack)
		print_forw_stack(out, temp->tab_data->forw_stack->head);

	fprintf(out, "%s\n", temp->tab_data->curr_page->url);

	if (!temp->tab_data->back_stack)
		return;

	stack_node_t *node = temp->tab_data->back_stack->head;
	while (node) {
		fprintf(out, "%s\n", node->data->url);
		node = node->next;
	}
}

page_t *read_page(FILE *in, int *n)
{
	fscanf(in, "%d", n);

	page_t *page = malloc((*n + 1) * sizeof(page_t));
	if (!page) {
		fprintf(stderr, "Memory allocation failed\n");
		fclose(in);
		return NULL;
	}

	page[0].id = 0;
	strcpy(page[0].url, "https://acs.pub.ro/");
	page[0].descp = malloc(17 * sizeof(char));
	strcpy(page[0].descp, "Computer Science");

	for (int i = 1; i <= *n; i++) {
		fscanf(in, "%d %50s", &page[i].id, page[i].url);

		fgetc(in); // Comsumam new line-ul dupa url

		char buffer[MAX_BUFF];
		fgets(buffer, sizeof(buffer), in);
		buffer[strcspn(buffer, "\n")] = 0; // Eliminam new line-ul de la sfarsit
		page[i].descp = malloc(strlen(buffer) + 1);
		strcpy(page[i].descp, buffer);
	}

	return page;
}

void free_browser(browser_t *browser)
{
	if (!browser)
		return;

	tabList_t *sentinel = browser->list;
	if (!sentinel) {
		free(browser);
		return;
	}

	tabList_t *node = sentinel->next;

	// Parcurgem lista circulară și eliberăm fiecare nod
	while (node != sentinel) {
		tabList_t *next = node->next;

		if (node->tab_data) {

			// Eliberăm back_stack
			free_stack(node->tab_data->back_stack);

			// Eliberăm forw_stack
			free_stack(node->tab_data->forw_stack);

			free(node->tab_data);
		}

		free(node);
		node = next;
	}

	// Eliberăm santinela
	free(sentinel);

	// Eliberăm browser-ul
	free(browser);
}

void read_instr(FILE *in, page_t *page, int n)
{
	FILE *out = fopen("tema1.out", "w");
	int m, id = 0;
	char line[MAX_BUFF];

	browser_t *browser = create_browser();
	add_tab(browser, &page[0], 0);

	fscanf(in, "%d", &m);
	fgetc(in); // Comsumam new line-ul dupa m
	for (int i = 0; i < m; i++) {

		fgets(line, sizeof(line), in);
		line[strcspn(line, "\n")] = 0;
		int len = strlen(line);
		while (len > 0 && line[len - 1] == ' ') {
			line[len - 1] = '\0';
			len--;
		}

		if (!strcmp(line, "NEW_TAB")) {
			id++;
			add_tab(browser, &page[0], id);

		} else if (!strcmp(line, "CLOSE")) {
			close_tab(out, browser);

		} else if (!strcmp(line, "NEXT")) {
			next_tab(out, browser);

		} else if (!strcmp(line, "PREV")) {
			prev_tab(out, browser);

		} else if (!strcmp(line, "BACKWARD")) {
			back_page(out, browser->curr);

		} else if (!strcmp(line, "FORWARD")) {
			next_page(out, browser->curr);

		} else if (!strcmp(line, "PRINT")) {
			f_print(out, browser);

		} else {
			char command[20]; // Pentru a retine comanda principala
			int arg1; // Argumentul aditional comenzii
			if (sscanf(line, "%s %d", command, &arg1) == 2) {
				if (!strcmp(command, "OPEN")) {
					open_tab(out, browser, arg1);

				} else if (!strcmp(command, "PAGE")) {
					swap_page(out, browser, page, arg1, n);

				} else if (!strcmp(command, "PRINT_HISTORY")) {
					history_print(out, browser, arg1);

				} else {
					fprintf(out, "Unknown command\n");
				}

			} else {
				fprintf(out, "Unknown command\n");
			}
		}

	}
	free_browser(browser);
	fclose(out);
}

void free_page(page_t *page, int n)
{
	for (int i = 0; i <= n; i++) {
		if (page[i].descp) {
			free(page[i].descp); // Eliberează descrierea
		}
	}

	free(page); // Eliberează vectorul de pagini
}

int main(void)
{
	FILE *in = fopen("tema1.in", "rt");
	if (!in) {
		fprintf(stderr, "Failed to load input file.\n");
		return 1;
	}

	int n;
	page_t *page = read_page(in, &n);
	read_instr(in, page, n);
	fclose(in);

	free_page(page, n);
	return 0;
}
