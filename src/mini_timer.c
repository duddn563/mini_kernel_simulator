#include "mini_timer.h"
#include "mini_printk.h"

#include <stdio.h>
#include <string.h>

typedef struct mini_timer {
	int active;
	char name[MINI_TIMER_NAME_SIZE];
	int expires_tick;
	mini_timer_handler_t handler;
} mini_timer_t;

static mini_timer_t timer_table[MINI_TIMER_MAX_COUNT];
static int current_tick_count;

void mini_timer_init(void)
{
	int i;

	current_tick_count = 0;

	for (i = 0; i < MINI_TIMER_MAX_COUNT; i++) {
		timer_table[i].active = 0;
		timer_table[i].name[0] = '\0';
		timer_table[i].expires_tick = 0;
		timer_table[i].handler = NULL;
	}

	mini_printk(
		MINI_LOG_INFO,
		"mini_timer initialized: slots=%d",
		MINI_TIMER_MAX_COUNT
	);
}

int mini_timer_add(const char *name, int expires_after_ticks, mini_timer_handler_t handler)
{
	int i;

	if (name == NULL) {
		mini_printk(
			MINI_LOG_ERROR,
			"mini_timer_add failed: name is NULL"
		);

		return -1;
	}

	if (handler == NULL) {
		mini_printk(
			MINI_LOG_ERROR,
			"mini_timer_add failed: handler is NULL"
		);

		return -1;
	}

	if (expires_after_ticks <= 0) {
		mini_printk(
			MINI_LOG_ERROR,
			"mini_timer_add failed: invalid expires_after_ticks=%d",
			expires_after_ticks
		);

		return -1;
	}

	for (i = 0; i < MINI_TIMER_MAX_COUNT; i++) {
		if (timer_table[i].active == 0) {
			timer_table[i].active = 1;

			strncpy(
				timer_table[i].name,
				name,
				MINI_TIMER_NAME_SIZE - 1
			);

			timer_table[i].expires_tick = 
				current_tick_count + expires_after_ticks;
		
			timer_table[i].handler = handler;

			mini_printk(
				MINI_LOG_INFO,
				"timer registered: slot=%d, name=%s, current_tick=%d, expires_tick=%d",
				i,
				timer_table[i].name,
				current_tick_count,
				timer_table[i].expires_tick
			);

			return i;
		}
	}

	mini_printk(
		MINI_LOG_ERROR,
		"mini_timer_add failed: no free timer slot"
	);

	return -1;
}

void mini_timer_tick(void)
{
	int i;

	current_tick_count++;

	printf("[TIMER] tick=%d\n", current_tick_count);

	mini_printk(
		MINI_LOG_INFO,
		"timer tick: current_tick=%d",
		current_tick_count
	);

	for (i = 0; i < MINI_TIMER_MAX_COUNT; i++) {
		if (timer_table[i].active == 0) {
			continue;
		}

		if (current_tick_count >= timer_table[i].expires_tick) {
			printf(
				"[TIMER] expired: name=%s, slot=%d\n",
				timer_table[i].name,
				i
			);

			mini_printk(
				MINI_LOG_INFO,
				"timer expired: slot=%d, name=%s",
				i,
				timer_table[i].name
			);

			timer_table[i].handler(timer_table[i].name);

			timer_table[i].active = 0;
			timer_table[i].name[0] = '\0';
			timer_table[i].expires_tick = 0;
			timer_table[i].handler = NULL;
		}
	}
}

void mini_timer_show_all(void)
{
	int i;

	printf("\n===== Timer Table =====\n");

	for (i = 0; i < MINI_TIMER_MAX_COUNT; i++) {
		if (timer_table[i].active != 0) {
			printf(
				"slot=%d, name=%s, active=%d, expires_tick=%d, current_tick=%d, remaining=%d\n",
				i,
				timer_table[i].name,
				timer_table[i].active,
				timer_table[i].expires_tick,
				current_tick_count,
				timer_table[i].expires_tick - current_tick_count
			);
		}
	}

	printf("========================\n");

	mini_printk(
		MINI_LOG_INFO,
		"timer table displayed"
	);
}
