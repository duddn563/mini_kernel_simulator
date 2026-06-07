#ifndef MINI_LIST_H
#define MINI_LIST_H

#include <stddef.h>

typedef struct mini_list_head {
	struct mini_list_head *prev;
	struct mini_list_head *next;
} mini_list_head_t;

#define mini_container_of(ptr, type, member) \
		((type *)((char *)(ptr) - offsetof(type, member)))

void mini_init_list_head(mini_list_head_t *list);
void mini_list_add_tail(mini_list_head_t *new_node, mini_list_head_t *head);
void mini_list_del(mini_list_head_t *entry);
int mini_list_empty(mini_list_head_t *head);
void mini_list_show_raw(mini_list_head_t *head);

#endif
