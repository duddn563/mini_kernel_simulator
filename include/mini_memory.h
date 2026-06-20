#ifndef MINI_MEMORY_H
#define MINI_MEMORY_H

#define MINI_MEMORY_BLOCK_SIZE 64
#define MINI_MEMORY_BLOCK_COUNT 8

#include <stdio.h>

void mini_memory_init(void);
void *mini_kmalloc(size_t size);
void mini_kfree(void *ptr);
void mini_memory_show_status(void);

#endif
