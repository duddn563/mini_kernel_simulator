#ifndef MINI_TASK_H
#define MINI_TASK_H

#define MINI_TASK_NAME_SIZE 32

typedef enum {
	MINI_TASK_READY = 0,
	MINI_TASK_RUNNING,
	MINI_TASK_SLEEPING,
	MINI_TASK_DONE
} mini_task_state_t;

typedef struct mini_task {
	int pid;
	char name[MINI_TASK_NAME_SIZE];
	mini_task_state_t state;
	struct mini_task *next;
} mini_task_t;

void mini_task_init(void);
mini_task_t *mini_task_create(int pid, const char *name, mini_task_state_t state);
void mini_task_add(mini_task_t *new_task);
void mini_task_show_all(void);
void mini_task_destroy_all(void);
mini_task_t *mini_task_get_head(void);

#endif
