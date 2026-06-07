#include "mini_scheduler.h"
#include "mini_task.h"
#include "mini_printk.h"

#include <stdio.h>

static mini_list_head_t *scheduler_ready_queue_head = NULL;
static mini_list_head_t *scheduler_current_node = NULL;

static const char *mini_scheduler_state_to_string(mini_task_state_t state)
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

static int mini_scheduler_is_runnable(mini_task_t *task)
{
	return task->state == MINI_TASK_READY;
}

void mini_scheduler_init(mini_list_head_t *ready_queue_head)
{
	scheduler_ready_queue_head = ready_queue_head;

	if (scheduler_ready_queue_head == NULL) {
		scheduler_current_node = NULL;
		mini_printk(MINI_LOG_WARN, "scheduler initialized with NULL ready queue");
		return;
	}

	if (mini_list_empty(scheduler_ready_queue_head)) {
		scheduler_current_node = NULL;
		mini_printk(MINI_LOG_WARN, "scheduler initialized with empty ready queue");
		return;
	}

	scheduler_current_node = scheduler_ready_queue_head->next;

	mini_printk(MINI_LOG_INFO, "scheduler initialized");
}

void mini_scheduler_run_once(void)
{
	mini_list_head_t *start_node = NULL;
	mini_task_t *task = NULL;

	if (scheduler_ready_queue_head == NULL) {
		mini_printk(MINI_LOG_WARN, "scheduler has no ready queue");
		return;
	}

	if (mini_list_empty(scheduler_ready_queue_head)) {
		mini_printk(MINI_LOG_WARN, "scheduler ready queue is empty");
		return;
	}

	if (scheduler_current_node == NULL ||
		scheduler_current_node == scheduler_ready_queue_head) {
		scheduler_current_node = scheduler_ready_queue_head->next;		
	}

	start_node = scheduler_current_node;

	while (1) {
		if (scheduler_current_node == scheduler_ready_queue_head) {
			scheduler_current_node = scheduler_ready_queue_head->next;
		}

		task = mini_container_of(scheduler_current_node,
														 mini_task_t,
														 task_node);

		if (mini_scheduler_is_runnable(task)) {
			task->state = MINI_TASK_RUNNING;

			mini_printk(MINI_LOG_INFO, 
									"scheduler running task: pid=%d, name=%s",
									task->pid,
									task->name);

			printf("[SCHED] running task: pid=%d, name=%s\n",
							task->pid,
							task->name);

			task->state = MINI_TASK_READY;

			scheduler_current_node = scheduler_current_node->next;

			if (scheduler_current_node == scheduler_ready_queue_head) {
				scheduler_current_node = scheduler_current_node->next;
			}

			return;
		}

		printf("[SCHED] skip task: pid=%d, name=%s, state=%s\n",
						task->pid,
						task->name,
						mini_scheduler_state_to_string(task->state));

		scheduler_current_node = scheduler_current_node->next;

		if (scheduler_current_node == start_node) {
			mini_printk(MINI_LOG_WARN, "scheduler found no runnable task");
			printf("[SCHED] no runnable task\n");
			return;
		}
	}
}

void mini_scheduler_run_rounds(int rounds)
{
	if (rounds <= 0) {
		mini_printk(MINI_LOG_WARN, "scheduler rounds must be greater than 0");
		return;
	}

	printf("===== Scheduler Run =====\n");

	for (int i = 0; i < rounds; i++) {
		mini_scheduler_run_once();
	}
	printf("=========================\n");
}
