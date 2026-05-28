#include "mini_printk.h"

int main(void)
{
	mini_printk_init();

	mini_printk(MINI_LOG_INFO, "mini kernel simulator start");
	mini_printk(MINI_LOG_DEBUG, "initializing mini_printk rint buffer");
	mini_printk(MINI_LOG_WARN, "this is a warning ring buffer");
	mini_printk(MINI_LOG_ERROR, "this is an error message");

	mini_printk(MINI_LOG_INFO, "test log 1");
	mini_printk(MINI_LOG_INFO, "test log 2");
	mini_printk(MINI_LOG_INFO, "test log 3");
	mini_printk(MINI_LOG_INFO, "test log 4");
	mini_printk(MINI_LOG_INFO, "test log 5");
	mini_printk(MINI_LOG_INFO, "test log 6");
	mini_printk(MINI_LOG_INFO, "test log 7");
	mini_printk(MINI_LOG_INFO, "test log 8");

	mini_printk_show_logs();
	return 0;
}
