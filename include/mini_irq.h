#ifndef MINI_IRQ_H
#define MINI_IRQ_H

#define MINI_IRQ_MAX_COUNT 16
#define MINI_IRQ_NAME_SIZE 32

typedef void (*mini_irq_handler_t)(int irq);

typedef struct {
	int irq;
	char name[MINI_IRQ_NAME_SIZE];
	mini_irq_handler_t handler;
	int registered;
} mini_irq_desc_t;

void mini_irq_init(void);
int mini_request_irq(int irq, const char *name, mini_irq_handler_t handler);
int mini_trigger_irq(int irq);
int mini_free_irq(int irq);
void mini_irq_show_table(void);

#endif
