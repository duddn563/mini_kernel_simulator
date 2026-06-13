## mini_memory

# 1. Overview

mini_memory is a simplified fixed-block memory allocator.

This module was created to understand the basic concepts behind:

- memory pools
- dynamic memory allocation
- allocation metadata
- memory release
- kmalloc() and kfree() style interfaces

The implementation does not reproduce the complete Linux kernel memory allocator.

Instead, it manages a small memory pool containing eight fixed-size block

# 2. Memory Pool Structure

The memory pool contains eight blocks

Each block can store up to 64 bytes.

#define MINI_MEMORY_BLOCK_SIZE 64
#define MINI_MEMORY_BLOCK_COUNT 8

Each block is represented by the following structure:

typedef struct mini_memory_block {
	int used;
	size_t requested_size;
	unsigned char data[MINI_MEMORY_BLOCK_SIZE];
} mini_memory_block_t;

The fields have the following purposes:
	
	used
		Indicates whether the block is currently allocated.

	requested_size
		Stores the number of bytes requested by the caller.

	data
		The actual memory area returned to the caller.

	The complete memory pool is declared as:
	
	static mini_memory_block_t memory_pool[MINI_MEMORY_BLOCK_COUNT];

	Because it is declared with static, the memory pool is only accessible inside
	mini_memory.c.


# 3. Memory Initialization

The memory allocator in initialized using:

void mini_memory_init(void);

The function resets every memory block.

For each block:
- used is set to 0
- requested_size is set to 0
- the data area is cleared with memset()

Example:

memory_pool[i].used = 0;
memory_pool[i].requested_size = 0;

memset(memory_pool[i].data,
			 0,
			 sizeof(memory_pool[i].data));

used = 0, free = 8, total = 8

# 4. Memory Allocation

Memory is allocated using:

void *mini_kmalloc(size_t size);

The function first validates the requested size.

A request fails when:

- the requested size is 0
- the requested size is greater than 64 bytes
- all memory blocks are already in use

The allocation searches the memory pool from block 0.

for (i = 0; i < MINI_MEMORY_BLOCK_COUNT; i++) {
	if (memory_pool[i].used == 0) {
		 ...
	}
}

When free block is found:
1. the blcok is marked as used
2. the requested size si stored
3. the data area is cleared
4. the address of the data area is returned
	 memory_pool[i].used = 1;
	 memory_pool[i].requested_size = size;
	 return memory_pool[i].data;

The caller does not receive the address of the entire block structure.

The caller receives only the address of the data member

# 5. Memory Release

Memory is released using:

void mini_kfree(void *ptr);

The function searches for a block whose data address matches the pointer.

if (ptr == (void*)memory_pool[i].data) {
	...
}

When the matching block is found:
1. the data area is cleared
2. used is changed to 0
3. requested_size is changed to 0
	memset(memory_pool[i].data,
	0,
	sizeof(memory_pool[i].data));
	memory_pool[i].used = 0;
	memory_pool[i].requested = 0;


	The block can then be allocated again.

	The function also detects:

	- a NULL pointer
	- an already freed block
	- a pointer that does not belong to memory pool

## 6. Freed Block Reuse

The following allocations were performed:

memory1 = mini_kmalloc(16);
memory2 = mini_kmalloc(32);
memory3 = mini_kmalloc(64);

The result was:

block 0 -> 16 bytes
block 1 -> 32 bytes
block 2 -> 64 bytes

mini_kfree(memory2);

After that, a new 24-byte allocation was requested:

memory = mimi_kmalloc(24);

The allocator returned block again.

Example output:

mini_kfree: block=1, address=0x24ea8
mini_kmalloc: block=1, size=24, address=0x24ea8

The same address waw returned.

This confirms that a released memory block can be reused.

## 7. Memory Status

The current state of the memory pool is displayed using:

void mini_memory_show_status(void);

For allocated blocks, the function displays:

- block index
- allocation state
- requested size
- data address

Example:

block[0]: USED, requested=16, address=0x24e60
block[1]: USED, requested=24, address=0x24ea8
block[2]: USED, requested=64, address=0x24ef0
used=3, free=5, total=8

After all allocated blocks are released:

used=0, free=8, total=8


## 8. Difference from malloc

mini_kmalloc() and standard malloc() share some basic concepts.

Both:
- locate an available memory region
- mark the region as allocated
- return a pointer
- allow the region to be resued after it is freed

However, this implementation is much simpler.

# mini_kmalloc
- uses eight preallocated blocks
- every has a fixed size of 64 bytes
- searchs for the first free block
- cannot divide or combine blocks
- cannot request additional memory from the operating system
- clears the memory when allocating

# malloc
- supports many  different allocation size
- manages variable-sized free regions
- may split a large free regions
- may merge adjacent free regions
- handles memory alignment
- handler fragmentation
- may request additional memory from the operating system
- normally does not initialize allocated memory

Because mini_kmalloc() clears the selected block with memset(), this part of its behavior is closer to calloc().

## 9. Limitations

The current implementaion has several limitations
- maximum allocation size is 64 bytes
- only eight allocations can exist at the same time
- one allocation always consumes one complete block
- requesting one byte still uses a full 64-bytes block
- blocks cannot be combined
- thread synchronization is not  implemented
- memory alignment is not explicitly controlled
- double-free detection is limited to known block address
- this is not a real Linux kernel allocator

These limitations are intentional because the purpose is to study the basic allocator workflow.

## 10. What I Learned

Through mini_memory, I learned:
1. how a fixed-block memory pool is organized
2. how allocation metadata is stored
3. how an allocator finds a free block
4. how a pointer to the memory area is returned
5. how a block is released
6. how released memory can be reused
7. how invalid allocations and invalid frees can be detected
8. the basic differences between malloc(), calloc(), and a memory pool

## 11. Summary

The main flow of mini_memory is:

initialize memory pool
	->
find a free block
	->
mark the block as used
	->
return the data address
	->
release the block
	->
reuse the released block

The module provides a simplified introduction to memory allocation and kernel-style memory management interfaces.







