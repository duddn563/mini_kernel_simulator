#include "mini_printk.h"
#include "mini_task.h"

#include <string.h>
#include <stdio.h>
#include <stddef.h>

int main(void)
{
	mini_task_t *init_task = NULL;
	mini_task_t *worker_task = NULL;
	mini_task_t *logger_task = NULL;

	mini_printk_init();
	mini_task_init();

  mini_printk(MINI_LOG_INFO, "mini kernel simulator start");
	mini_printk(MINI_LOG_DEBUG, "initializing mini_printk ring buffer");

	init_task = mini_task_create(1, "init", MINI_TASK_RUNNING);
	worker_task = mini_task_create(2, "worker", MINI_TASK_READY);
	logger_task = mini_task_create(3, "logger", MINI_TASK_SLEEPING);

	mini_task_add(init_task);
	mini_task_add(worker_task);
	mini_task_add(logger_task);

	mini_task_show_all();

	mini_printk_show_logs();

	mini_task_destroy_all();

	return 0;
}
