#include "mini_task.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static mini_task_t *task_head = NULL;
static mini_list_head_t task_list_head;

const char *mini_task_state_to_string(mini_task_state_t state)
{
	switch (state) {
		case MINI_TASK_READY:
			return "READY";
		case MINI_TASK_RUNNING:
			return "RUNNING";
		case MINI_TASK_SLEEPING:
			return "SLEEPING";
		case MINI_TASK_DONE:
			return "DONE";
		default:
			return "UNKNOWN";
	}
}

void mini_task_init(void)
{
	task_head = NULL;
	mini_init_list_head(&task_list_head);
	mini_printk(MINI_LOG_INFO, "mini_task initialized");
}

mini_task_t *mini_task_create(int pid, const char *name, mini_task_state_t state)
{
	mini_task_t *new_task = NULL;

	new_task = (mini_task_t *)malloc(sizeof(mini_task_t));
	if (new_task == NULL) {
		mini_printk(MINI_LOG_ERROR, "failed to allocate memory for task");
		return NULL;
	}

	new_task->pid = pid;
	snprintf(new_task->name, MINI_TASK_NAME_SIZE, "%s", name);
	new_task->state = state;
	new_task->next = NULL;

	mini_list_add_tail(&new_task->task_node, &task_list_head);

	mini_printk(MINI_LOG_INFO, 
							"task created: pid=%d, name=%s, state=%s",
							new_task->pid,
							new_task->name,
							mini_task_state_to_string(new_task->state));

	return new_task;
}

void mini_task_add(mini_task_t *new_task)
{
	mini_task_t *current = NULL;

	if (new_task == NULL) {
		mini_printk(MINI_LOG_WARN, "cannot add NULL task");
		return;
	}

	if (task_head == NULL) {
		task_head = new_task;
		
		mini_printk(MINI_LOG_INFO,
								"task added as head: pid=%d, name=%s",
								new_task->pid,
								new_task->name);
		return;
	}

	current = task_head;

	while (current->next != NULL) {
		current = current->next;
	}

	current->next = new_task;

	mini_printk(MINI_LOG_INFO,
							"task added to list: pid=%d, name=%s",
							new_task->pid,
							new_task->name);
}

void mini_task_show_all(void)
{
	mini_task_t *current = NULL;	

	printf("\n===== Task List =====\n");

	if (task_head == NULL) {
		printf("No tasks\n");
		printf("=====================\n");
		return;
	}

	current = task_head;

	while (current != NULL) {
		printf("pid=%d, name=%s, state=%s, next=%p\n",
						current->pid,
						current->name,
						mini_task_state_to_string(current->state),
						(void *)current->next);
		current = current->next;
	}
	printf("=====================\n");

	mini_list_head_t *current_list = task_list_head.next;

	printf("\n===== Task double linked List =====\n");

	if (task_head == NULL) {
		printf("No tasks\n");
		printf("=====================\n");
		return;
	}

	while (current_list != &task_list_head) {
		mini_task_t *task = mini_container_of(current_list, mini_task_t, task_node);

		printf("pid=%d, name=%s, state=%s, node=%p, prev=%p, next=%p\n",
						task->pid,
						task->name,
						mini_task_state_to_string(task->state),
						(void *)&task->task_node,
						(void *)task->task_node.prev,
						(void *)task->task_node.next);
		current_list = current_list->next;
	}

	printf("=====================\n");

	mini_list_show_raw(&task_list_head);
}

void mini_task_destroy_all(void)
{
	mini_task_t *current = task_head;
	mini_task_t *next_task = NULL;

	while (current != NULL) {
		 next_task = current->next;

		 mini_printk(MINI_LOG_INFO,
		 						 "task destroyed: pid=%d, name=%s",
								 current->pid,
								 current->name);
		 free(current);
		 current = next_task;
	}

	task_head = NULL;
}

mini_task_t *mini_task_get_head(void)
{
	return task_head;
}
