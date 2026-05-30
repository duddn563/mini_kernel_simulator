#ifndef MINI_SCHEDULER_H
#define MINI_SCHEDULER_H

#include "mini_task.h"

void mini_scheduler_init(mini_task_t *ready_queue_head);
void mini_scheduler_run_once(void);
void mini_scheduler_run_rounds(int rounds);

#endif
