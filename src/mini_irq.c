#include "mini_irq.h"
#include "mini_printk.h"

#include <stdio.h>
#include <string.h>

static mini_irq_desc_t irq_table[MINI_IRQ_MAX_COUNT];

static int mini_irq_is_valid(int irq)
{
	return irq > 0 && irq < MINI_IRQ_MAX_COUNT;
}

void mini_irq_init(void)
{
	memset(irq_table, 0, sizeof(irq_table));

	for (int i = 0; i < MINI_IRQ_MAX_COUNT; i++) {
		irq_table[i].irq = i;
		irq_table[i].registered = 0;
		irq_table[i].handler = NULL;
		snprintf(irq_table[i].name, MINI_IRQ_NAME_SIZE, "%s", "unused");
	}

	mini_printk(MINI_LOG_INFO, "mini_irq initialized");
}

int mini_request_irq(int irq, const char *name, mini_irq_handler_t handler)
{
	if (!mini_irq_is_valid(irq)) {
		mini_printk(MINI_LOG_ERROR, "invalid irq number: irq=%d", irq);
		return -1;
	}

	if (handler == NULL) {
		mini_printk(MINI_LOG_ERROR, "cannot register NULL irq handler: irq=%d", irq);
		return -1;
	}

	if (irq_table[irq].registered) {
		mini_printk(MINI_LOG_WARN, "irq already registerd: irq=%d, name=%s",
								irq,
								irq_table[irq].name);
		return -1;
	}

	irq_table[irq].registered = 1;
	irq_table[irq].handler = handler;
	snprintf(irq_table[irq].name, MINI_IRQ_NAME_SIZE, "%s", name);

	mini_printk(MINI_LOG_INFO,
							"irq registered: irq=%d, name=%s",
							irq,
							irq_table[irq].name);

	return 0;
}

int mini_trigger_irq(int irq)
{
	if (!mini_irq_is_valid(irq)) {
		mini_printk(MINI_LOG_ERROR, "invalid irq trigger: irq=%d", irq);
		return -1;
	}

	if (!irq_table[irq].registered || irq_table[irq].handler == NULL) {
		mini_printk(MINI_LOG_WARN, "irq not registered: irq=%d", irq);
		return -1;
	}

	mini_printk(MINI_LOG_INFO,
							"irq triggered: irq=%d, name=%s",
							irq,
							irq_table[irq].name);
	printf("[IRQ] triggered: irq=%d, name=%s\n",
					irq,
					irq_table[irq].name);

	irq_table[irq].handler(irq);

	return 0;
}

int mini_free_irq(int irq)
{
	if (!mini_irq_is_valid(irq)) {
		mini_printk(MINI_LOG_ERROR, "invalid irq free: irq=%d", irq);
		return -1;
	}

	if (!irq_table[irq].registered) {
		mini_printk(MINI_LOG_WARN, "cannot free unregistered irq: irq=%d", irq);
		return -1;
	}
	
	mini_printk(MINI_LOG_INFO, 
							"irq freed: irq=%d, name=%s",
							irq,
							irq_table[irq].name);

	irq_table[irq].registered = 0;
	irq_table[irq].handler = NULL;
	snprintf(irq_table[irq].name, MINI_IRQ_NAME_SIZE, "%s", "unused");

	return 0;
}


void mini_irq_show_table(void)
{
	printf("\n===== IRQ Table =====\n");

	for (int i = 0; i < MINI_IRQ_MAX_COUNT; i++) {
		if (irq_table[i].registered) {
			printf("irq=%d, name=%s, handler=%p, registered=%d\n",
							irq_table[i].irq,
							irq_table[i].name,
							(void *)irq_table[i].handler,
							irq_table[i].registered);
		}
	}

	printf("======================\n");
}


