#include "mini_scheduler.h"
#include "mini_printk.h"

#include <stdio.h>

static mini_task_t *scheduler_ready_queue_head = NULL;
static mini_task_t *scheduler_current_task = NULL;

void mini_scheduler_init(mini_task_t *ready_queue_head)
{
	scheduler_ready_queue_head = ready_queue_head;
	scheduler_current_task = ready_queue_head;

	if (scheduler_ready_queue_head == NULL) {
		mini_printk(MINI_LOG_WARN, "scheduler initialized with empty ready queue");
		return;
	}

	mini_printk(MINI_LOG_INFO, "scheduler initialized");
}

void mini_scheduler_run_once(void)
{
	if (scheduler_ready_queue_head == NULL) {
		mini_printk(MINI_LOG_WARN, "scheduler has no task to run");
		return;
	}

	if (scheduler_current_task == NULL) {
		scheduler_current_task = scheduler_ready_queue_head;
	}

	scheduler_current_task->state = MINI_TASK_RUNNING;

	mini_printk(MINI_LOG_INFO,
							"scheduler running task: pid=%d, name=%s",
							scheduler_current_task->pid,
							scheduler_current_task->name);

	printf("[SCHED] running task: pid=%d, name=%s\n",
					scheduler_current_task->pid,
					scheduler_current_task->name);
	
	scheduler_current_task->state = MINI_TASK_READY;

	if (scheduler_current_task->next != NULL) {
		scheduler_current_task = scheduler_current_task->next;
	}
	else {
		scheduler_current_task = scheduler_ready_queue_head;
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
