#ifndef MINI_PRINTK_H
#define MINI_PRINTK_H

#define MINI_LOG_BUFFER_SIZE 10
#define MINI_LOG_MESSAGE_SIZE 256

typedef enum {
	MINI_LOG_INFO = 0,
	MINI_LOG_WARN,
	MINI_LOG_ERROR,
	MINI_LOG_DEBUG
} mini_log_level_t;

typedef struct {
	mini_log_level_t level;
	char message[MINI_LOG_MESSAGE_SIZE];
} mini_log_entry_t;

void mini_printk_init(void);
void mini_printk(mini_log_level_t level, const char *format, ...);
void mini_printk_show_logs(void);

#endif
