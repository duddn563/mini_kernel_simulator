#include "mini_printk.h"
#include "mini_task.h"
#include "mini_scheduler.h"
#include "mini_irq.h"
#include "mini_device.h"

#include <stdio.h>

static void timer_irq_handler(int irq)
{
	printf("[HANDLER] timer interrupt handler called: irq=%d\n", irq);
	mini_printk(MINI_LOG_DEBUG, "timer irq handler executed: irq=%d", irq);
}

static void keyboard_irq_handler(int irq)
{
	printf("[HANDLER] keyboard interrupt handler called: irq=%d\n", irq);
	mini_printk(MINI_LOG_DEBUG, "keyboard irq handler executed: irq=%d", irq);
}

int main(void)
{
	mini_task_t *init_task = NULL;
	mini_task_t *worker_task = NULL;
	mini_task_t *logger_task = NULL;

	mini_file_operations_t *device_fops = NULL;
	char read_buffer[MINI_DEVICE_BUFFER_SIZE];

	mini_printk_init();
	mini_task_init();
	mini_irq_init();
	mini_device_init();

  mini_printk(MINI_LOG_INFO, "mini kernel simulator start");
	mini_printk(MINI_LOG_DEBUG, "initializing mini_printk ring buffer");

	init_task = mini_task_create(1, "init", MINI_TASK_READY);
	worker_task = mini_task_create(2, "worker", MINI_TASK_READY);
	logger_task = mini_task_create(3, "logger", MINI_TASK_READY);

	mini_task_add(init_task);
	mini_task_add(worker_task);
	mini_task_add(logger_task);

	mini_task_show_all();

	mini_scheduler_init(mini_task_get_head());
	mini_scheduler_run_rounds(6);

	mini_request_irq(1, "timer", timer_irq_handler);
	mini_request_irq(2, "keyboard", keyboard_irq_handler);

	mini_irq_show_table();

	mini_trigger_irq(1);
	mini_trigger_irq(2);
	mini_trigger_irq(3);
	
	mini_free_irq(1);
	mini_free_irq(2);

	mini_irq_show_table();

	device_fops = mini_device_get_fops();

	device_fops->open();
	device_fops->write("hello mini device", 17);
	device_fops->read(read_buffer, sizeof(read_buffer));
	device_fops->close();

	mini_printk_show_logs();


	mini_task_destroy_all();

	return 0;
}
