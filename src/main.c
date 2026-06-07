#include "mini_printk.h"
#include "mini_task.h"
#include "mini_scheduler.h"
#include "mini_irq.h"
#include "mini_device.h"

#include <stdio.h>

typedef struct mini_list_test_node {
	int id;
	char name[32];
	mini_list_head_t list;
} mini_list_test_node_t;

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
	mini_file_operations_t *device_fops = NULL;
	char read_buffer[MINI_DEVICE_BUFFER_SIZE];

	mini_list_head_t test_list;
	mini_list_head_t node1;
	mini_list_head_t node2;
	mini_list_head_t node3;
	mini_list_head_t node4;

	mini_printk_init();
	mini_task_init();
	mini_irq_init();
	mini_device_init();

	mini_init_list_head(&test_list);
	mini_init_list_head(&node1);
	mini_init_list_head(&node2);
	mini_init_list_head(&node3);
	mini_init_list_head(&node4);

  mini_printk(MINI_LOG_INFO, "mini kernel simulator start");
	mini_printk(MINI_LOG_DEBUG, "initializing mini_printk ring buffer");

	mini_task_create(1, "init", MINI_TASK_READY);
	mini_task_create(2, "worker", MINI_TASK_SLEEPING);
	mini_task_create(3, "logger", MINI_TASK_READY);

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

	mini_printk(MINI_LOG_INFO, "mini_list test start");

	mini_list_add_tail(&node1, &test_list);
	mini_list_add_tail(&node2, &test_list);
	mini_list_add_tail(&node3, &test_list);
	mini_list_add_tail(&node4, &test_list);

	mini_list_show_raw(&test_list);

	mini_list_del(&node2);
	 
	mini_list_show_raw(&test_list);

	mini_printk_show_logs();

	return 0;
}
