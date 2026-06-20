#include "mini_printk.h"
#include "mini_task.h"
#include "mini_scheduler.h"
#include "mini_irq.h"
#include "mini_device.h"
#include "mini_memory.h"
#include "mini_timer.h"

#include <stdio.h>
#include <string.h>

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

static void mini_timer_a_handler(const char *timer_name)
{
	printf(
		"[HANDLER] %s handler run\n",
		timer_name
	);

	mini_printk(
		MINI_LOG_INFO,
		"%s handler executed",
		timer_name
	);
}

static void mini_timer_b_handler(const char *timer_name)
{
	printf(
		"[HANDLER] %s handler run\n",
		timer_name
	);
	
	mini_printk(
		MINI_LOG_INFO,
		"%s handler executed",
		timer_name
	);
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

	char *memory1 = NULL;
	char *memory2 = NULL;
	char *memory3 = NULL;
	char *memory4 = NULL;

	mini_printk_init();
	mini_task_init();
	mini_irq_init();
	mini_device_init();
	mini_memory_init();
	mini_timer_init();

	mini_init_list_head(&test_list);
	mini_init_list_head(&node1);
	mini_init_list_head(&node2);
	mini_init_list_head(&node3);
	mini_init_list_head(&node4);

  mini_printk(MINI_LOG_INFO, "mini kernel simulator start");
	mini_printk(MINI_LOG_DEBUG, "initializing mini_printk ring buffer");

	/*
	 * mini_task / mini_scheduler test
	 */
	mini_task_create(1, "init", MINI_TASK_READY);
	mini_task_create(2, "worker", MINI_TASK_SLEEPING);
	mini_task_create(3, "logger", MINI_TASK_READY);

	mini_task_show_all();

	mini_scheduler_init(mini_task_get_head());
	mini_scheduler_run_rounds(6);

	/*
   * mini_irq test
	 */
	mini_request_irq(1, "timer", timer_irq_handler);
	mini_request_irq(2, "keyboard", keyboard_irq_handler);

	mini_irq_show_table();

	mini_trigger_irq(1);
	mini_trigger_irq(2);
	mini_trigger_irq(3);
	
	mini_free_irq(1);
	mini_free_irq(2);

	mini_irq_show_table();

	/*
   * mini_device test
	 */
	device_fops = mini_device_get_fops();

	device_fops->open();
	device_fops->write("hello mini device", 17);
	device_fops->read(read_buffer, sizeof(read_buffer));
	device_fops->close();


	/*
	 * mini_list test
	 */
	mini_printk(MINI_LOG_INFO, "mini_list test start");

	mini_list_add_tail(&node1, &test_list);
	mini_list_add_tail(&node2, &test_list);
	mini_list_add_tail(&node3, &test_list);
	mini_list_add_tail(&node4, &test_list);

	mini_list_show_raw(&test_list);

	mini_list_del(&node2);
	 
	mini_list_show_raw(&test_list);

	/*
	 * mini_memory test
	 */
	mini_printk(
		MINI_LOG_INFO,
		"mini memory test start"
	);

	mini_memory_show_status();

	memory1 = mini_kmalloc(16);
	memory2 = mini_kmalloc(32);
	memory3 = mini_kmalloc(64);

	if (memory1 != NULL) {
		strcpy(
			memory1,
			"memory block 1"
		);
	}

	if (memory2 != NULL) {
		strcpy(
			memory2,
			"memory block 2"
		);
	}

	if (memory3 != NULL) {
		strcpy(
			memory3,
			"memory block 3"
		);
	}

	mini_printk(
		MINI_LOG_INFO,
		"memory1 data: %s",
		memory1 != NULL ? memory1 : "NULL"
	);

	mini_printk(
		MINI_LOG_INFO,
		"memory2 data: %s",
		memory2 != NULL ? memory2 : "NULL"
	);
	
	mini_printk(
		MINI_LOG_INFO,
		"memory3 data: %s",
		memory3 != NULL ? memory3 : "NULL"
	);

	mini_memory_show_status();

	/*
	 * 두 번쨰 블록 반납
	 */
	mini_kfree(memory2);
	memory2 = NULL;

	mini_memory_show_status();

	/*
	 * 반납된 블록이 다시 사용되는지확인 
	 */
	memory4 = mini_kmalloc(24);

	if (memory4 != NULL) {
		strcpy(memory4, "reused memory block");
	}

	mini_printk(
		MINI_LOG_INFO,
		"memory4 data: %s",
		memory4 != NULL ? memory4 : "NULL"
	);

	mini_memory_show_status();

	mini_kfree(memory1);
	mini_kfree(memory3);
	mini_kfree(memory4);

	memory1 = NULL;
	memory3 = NULL;
	memory4 = NULL;

	mini_memory_show_status();

	/*
	 *	mini_timer test
	 */
	mini_printk(
		MINI_LOG_INFO,
		"mini_timer test start"
	);

	mini_timer_add(
		"timer_a",
		3,
		mini_timer_a_handler
	);

	mini_timer_add(
		"timer_b",
		5,
		mini_timer_b_handler
	);

	mini_timer_show_all();

	mini_timer_tick();
	mini_timer_show_all();

	mini_timer_tick();
	mini_timer_show_all();

	mini_timer_tick();
	mini_timer_show_all();

	mini_timer_tick();
	mini_timer_show_all();

	mini_timer_tick();
	mini_timer_show_all();

	mini_timer_tick();
	mini_timer_show_all();

	/*
	 *	모든 로그 출력
	 */

	mini_timer_show_all();



	/*
	 * 모든 로그 출력 
	 */
	mini_printk_show_logs();

	return 0;
}
