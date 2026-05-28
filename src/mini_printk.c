#include "mini_printk.h"

#include <stdio.h>
#include <stdarg.h>
#include <string.h>

static mini_log_entry_t log_buffer[MINI_LOG_BUFFER_SIZE];
static int log_start = 0;
static int log_count = 0;

static const char *mini_log_level_to_string(mini_log_level_t level)
{
	switch(level) {
		case MINI_LOG_INFO:
			return "INFO";
		case MINI_LOG_WARN:
			return "WARN";
		case MINI_LOG_ERROR:
			return "ERROR";
		case MINI_LOG_DEBUG:
			return "DEBUG";
		default:
			return "UNKNOWN";
	}
}

void mini_printk_init(void)
{
	memset(log_buffer, 0, sizeof(log_buffer));
	log_start = 0;
	log_count = 0;
}

void mini_printk(mini_log_level_t level, const char *format, ...)
{
	va_list args;
	char temp_message[MINI_LOG_MESSAGE_SIZE];

	va_start(args, format);
	vsnprintf(temp_message, sizeof(temp_message), format, args);
	va_end(args);

	int write_index = (log_start + log_count) % MINI_LOG_BUFFER_SIZE;

	if (log_count == MINI_LOG_BUFFER_SIZE) {
		write_index = log_start;
		log_start = (log_start + 1) % MINI_LOG_BUFFER_SIZE;
	}
	else {
		log_count++;
	}

	log_buffer[write_index].level = level;
	snprintf(log_buffer[write_index].message,
					 MINI_LOG_MESSAGE_SIZE,
					 "%s",
					 temp_message);

	printf("[%s] %s\n", mini_log_level_to_string(level), temp_message);
}

void mini_printk_show_logs(void)
{
	printf("\n===== kernel log Buffer ====\n");

	for (int i = 0; i < log_count; i++) {
		int index = (log_start + i) % MINI_LOG_BUFFER_SIZE;

		printf("[%s] %s\n", mini_log_level_to_string(log_buffer[index].level),
						log_buffer[index].message);
	}

	printf("=============================\n");
}


