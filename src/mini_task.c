#include "mini_task.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
	mini_init_list_head(&task_list_head);

	mini_printk(
			MINI_LOG_INFO, 
			"mini_task initialized"
	);
}

mini_task_t *mini_task_create(int pid, const char *name, mini_task_state_t state)
{
	mini_task_t *new_task = NULL;

	new_task = (mini_task_t *)malloc(sizeof(mini_task_t));
	if (new_task == NULL) {
		mini_printk(
				MINI_LOG_ERROR, 
				"failed to allocate memory for task"
		);

		return NULL;
	}

	new_task->pid = pid;

	snprintf(
			new_task->name, 
			MINI_TASK_NAME_SIZE,
			"%s", 
			name
	);

	new_task->state = state;

	mini_init_list_head(
			&new_task->task_node
	);

	mini_list_add_tail(
			&new_task->task_node, 
			&task_list_head
	);

	mini_printk(
			MINI_LOG_INFO, 
			"task created: pid=%d, name=%s, state=%s",
			new_task->pid,
			new_task->name,
			mini_task_state_to_string(new_task->state)
	);

	return new_task;
}

mini_task_t *mini_task_find_by_pid(int pid)
{
	mini_list_head_t *current_list = task_list_head.next;

	while (current_list != &task_list_head) {
			mini_task_t *task = mini_container_of(
					current_list,
					mini_task_t,
					task_node
			);

			if (task->pid == pid) {
					return task;
			}

			current_list = current_list->next;
	}

	return NULL;
}

int mini_task_set_state(int pid, mini_task_state_t state)
{
		mini_task_t *task = NULL;
		mini_task_state_t old_state;

		task = mini_task_find_by_pid(pid);
		if (task == NULL) {
				mini_printk(
						MINI_LOG_ERROR,
						"task state change failed: pid=%d not found",
						pid
				);

				return -1;
		}

		old_state = task->state;
		task->state = state;

		mini_printk(
				MINI_LOG_INFO,
				"task state changed: pid=%d, name=%s, %s->%s",
				task->pid,
				task->name,
				mini_task_state_to_string(old_state),
				mini_task_state_to_string(task->state)
		);

		return 0;
}

void mini_task_show_all(void)
{
	mini_list_head_t *current_list = task_list_head.next;

	printf("\n===== Task double linked List =====\n");

	if (mini_list_empty(&task_list_head)) {
		printf("No tasks\n");
		printf("=====================\n");
		return;
	}

	while (current_list != &task_list_head) {
		mini_task_t *task = mini_container_of(
				current_list, 
				mini_task_t, 
				task_node
		);

		printf("pid=%d, name=%s, state=%s, node=%p, prev=%p, next=%p\n",
						task->pid,
						task->name,
						mini_task_state_to_string(task->state),
						(void *)&task->task_node,
						(void *)task->task_node.prev,
						(void *)task->task_node.next
		);

		current_list = current_list->next;
	}

	printf("=====================\n");

	mini_list_show_raw(&task_list_head);
}

mini_list_head_t *mini_task_get_head(void)
{
	return &task_list_head;
}
