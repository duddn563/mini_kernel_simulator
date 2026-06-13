#include "mini_memory.h"
#include "mini_printk.h"

#include <stddef.h>
#include <string.h>

typedef struct mini_memory_block {
	int used;
	size_t requested_size;
	unsigned char data[MINI_MEMORY_BLOCK_SIZE];
} mini_memory_block_t;

static mini_memory_block_t memory_pool[MINI_MEMORY_BLOCK_COUNT];

void mini_memory_init(void)
{
	int i;

	for (i = 0; i < MINI_MEMORY_BLOCK_COUNT; i++) {
		memory_pool[i].used = 0;
		memory_pool[i].requested_size = 0;

		memset(memory_pool[i].data,
					 0,
					 sizeof(memory_pool[i].data));
	}

	mini_printk(
		MINI_LOG_INFO,
		"mini_memory initialized: blocks=%d, block_size=%d",
		MINI_MEMORY_BLOCK_COUNT,
		MINI_MEMORY_BLOCK_SIZE
	);
}

void *mini_kmalloc(size_t size)
{
	int i;

	if (size == 0) {
		mini_printk(
			MINI_LOG_WARN,
			"mini_kmalloc failed: requested size is 0"
		);

		return NULL;
	}

	if (size > MINI_MEMORY_BLOCK_SIZE) {
		mini_printk(
			MINI_LOG_ERROR,
			"mini_kmalloc failed: requested=%zu, maximum=%d",
			size,
			MINI_MEMORY_BLOCK_SIZE
		);

		return NULL;
	}

	for (i = 0; i < MINI_MEMORY_BLOCK_COUNT; i++) {
		if (memory_pool[i].used == 0) {
			memory_pool[i].used = 1;
			memory_pool[i].requested_size = size;

			memset(memory_pool[i].data,
						 0,
						 sizeof(memory_pool[i].data));

			mini_printk(
				MINI_LOG_INFO,
				"mini_kmalloc: block=%d, size=%zu, address=%p",
				i,
				size,
				(void*)memory_pool[i].data
			);

			return memory_pool[i].data;
		}
	}

	mini_printk(
		MINI_LOG_ERROR,
		"mini_kmalloc failed: no free memory block"
	);

	return NULL;
}

void mini_kfree(void *ptr)
{
	int i;

	if (ptr == NULL) {
		mini_printk(
			MINI_LOG_WARN,
			"mini_kfree ignored: pointer is NULL"
		);

		return;
	}

	for (i = 0; i < MINI_MEMORY_BLOCK_COUNT; i++) {
		if (ptr == (void*)memory_pool[i].data) {
			if (memory_pool[i].used == 0) {
				mini_printk(
					MINI_LOG_WARN,
					"mini_kfree ignored: block=%d is already free",
					i
				);

				return;
			}

			memset(memory_pool[i].data,
						 0,
						 sizeof(memory_pool[i].data));

			memory_pool[i].used = 0;
			memory_pool[i].requested_size = 0;

			mini_printk(
				MINI_LOG_INFO,
				"mini_kfree: block=%d, address=%p",
				i,
				ptr
			);

			return;
		}
	}

	mini_printk(
		MINI_LOG_ERROR,
		"mini_kfree failed: invalid address=%p",
		ptr
	);
}


void mini_memory_show_status(void)
{
	int i;
	int used_count = 0;

	mini_printk(
		MINI_LOG_INFO,
		"===== mini_memory status ====="
	);

	for (i = 0; i < MINI_MEMORY_BLOCK_COUNT; i++) {
		if (memory_pool[i].used != 0) {
			used_count++;

			mini_printk(
				MINI_LOG_INFO,
				"block[%d]: USED, requested=%zu, address=%p",
				i,
				memory_pool[i].requested_size,
				(void *)memory_pool[i].data
			);

		}
	}

	mini_printk(
		MINI_LOG_INFO,
		"used=%d, free=%d, total=%d",
		used_count,
		MINI_MEMORY_BLOCK_COUNT - used_count,
		MINI_MEMORY_BLOCK_COUNT
	);
}
