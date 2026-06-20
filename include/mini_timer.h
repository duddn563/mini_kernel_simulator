#ifndef MINI_TIMER_H
#define MINI_TIMER_H

#define MINI_TIMER_MAX_COUNT 8
#define MINI_TIMER_NAME_SIZE 32

typedef void (*mini_timer_handler_t)(const char *timer_name);

void mini_timer_init(void);
int mini_timer_add(const char *name, int expires_after_ticks, mini_timer_handler_t handler);
void mini_timer_tick(void);
void mini_timer_show_all(void);

#endif
