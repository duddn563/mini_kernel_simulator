# 06. mini_list

## Purpose

This document explains the `mini_list` module.

The goal of this module is to understand the Linux kernel-style `list_head` concept by implementing a simplified circular doubly linked list in user-space C.

The module is not the real Linux kernel list implementation.
It is a simplified learning module for understanding `next`, `prev`, circular list structure, and `container_of`.

## Kernel Concept

Linux kernel often users `struct list_head` to manage linked list.

Unlike a simple linked list that stores a direct `next` pointer inside each data structure, the kernel-style list stores a small list node inside the parent structure.

The list node is connected to other list nodes.

The parent structure can be recovered from the list node using `container_of`.

## mini_list_head_t

The  basig list node is defined as:

	typedef struct mini_list_haed {
		struct mini_list_head *next;
		struct mini_list_head *prev;
	} mini_list_head_t;

Each node has:
	next : pointer to the next list node
	prev : pointer to the previous list node

This creates a doubly linked list.

## Circular List

In this project, the list is circular

When the list is empty:
	
head->next = head
head->prev = head

So the head points to ifself.

After adding nodes:
head <-> node1 <-> node2 <-> node3 <-> head

The core idea is:
	
	new_node is inserted between bead->prev and head

The important connection logic is:
	
	static void mini_list_add_between(mini_list_head_t *new_node,
																		mini_list_head_t *prev,
																		mini_list_head_t *next)
	{
		next->prev = new_node;
		new_node->next = next;
		new_node->prev = prev;
		prev->next = new_node;
	}

For tail insertion:
	
	void mini_list_add_tail(mini_list_head_t *new_node, mini_list_head_t *head)
	{
		mini_list_add_between(new_node, head->prev, head);
	}

## mini_list_del()

`mini_list_del()` removes a node from the list.

If the list is:

	head <-> node1 <-> node2 <-> node3 <-> head

After deleting `node2`:

	head <-> node1 <-> node3 <-> head

The important logic is:

	prev->next = next;
	next->prev = prev;

After deletion, the removed node points to itself again.

	entry->next = entry;
	entry->prev = entry;

This makes the deleted node isolated from the list.

## mini_list_emtpy()

`mini_list_empty()` checks whether the list is empty

## mini_list_emtpy()

`mini_list_empty()` checks whether the list is empty.

	int mini_list_empty(mini_list_head_t *head)
	{
		return head->next == head;
	}

If `head->next` points to itself, there are no nodes in the list.  

## Raw List Test

I tested the raw circular doubly linked list.

Example output:

	===== Mini List Raw =====
	node[0]=0x7ec73988, prev=0x7ec73990, next=0x7ec73980
	node[1]=0x7ec73980, prev=0x7ec73988, next=0x7ec73978
	node[2]=0x7ec73978, prev=0x7ec73980, next=0x7ec73970
	node[3]=0x7ec73970, prev=0x7ec73978, next=0x7ec73990
	=========================

In this output:
	
	head = 0x7ec73990

The list structure is:

	head <-> node0 <-> node1 <-> node2 <-> node3 <-> head

After deleting `node1`:

	===== Mini List Raw =====
	node[0]=0x7ec73988, prev=0x7ec73990, next=0x7ec73980
	node[1]=0x7ec73978, prev=0x7ec73980, next=0x7ec73970
	node[2]=0x7ec73970, prev=0x7ec73978, next=0x7ec73990
	=========================

The list becomes:

	head <-> node0 <-> node1 <-> node3 <-> head

The confirms that	`mini_list_del()` correctly reconnects the previous and next nodes. 

## container_of

`container_of`is used to get the parent structure address from a member address.

In this project:
	
	#define mini_container_of(ptr, type, member) \
				((type*)((char*)(ptr) - offsetof(type, member)))

Example structure:

	typedef struct mini_list_test_node {
		int id;
		char name[32];
		mini_list_head_t list;
	} mini_list_test_node_t;

If we only have the address of `list`, we can recover the parent structure:

	mini_list_test_node_t *node;

	node = mini_container_of(list_ptr,
													 mini_list_test_t,
													 list);

The idea is:
	
	parent address = member address - member offset

## container_of Test

Example output:
	
	===== container_of Test =====
	test_node address				= 0x7eb119d0
	list member address			= 0x7eb119f4
	parent_node address			= 0x7eb119d0
	parent_node id					= 10
	parent_node name				= container_test
	=============================

The important result is:

	test_node address == parent_node address

This means `mini_container_of()` successfully recovered the parent structure address from the list member address.

The address difference was:

	0x7eb119f4 - 0x7eb119d0 = 0x24 = 36

This matches the structre layout:

	int id				= 4 bytes
	char name[32] = 32 bytes
	total before list = 36 bytes

So the `list` member starts 36 bytes after the beginning of the structure.

## Applying mini_list to mini_task

Before this module, `mini_task` used a direct `next` pointer.

	struct mini_task *next;

After applying `mini_list`, each task contains a list node.

	mini_list_head_t task_node;

The task list is connected list this:

	task_head <-> init.task_node <-> worker.task_node <-> logger.task_node <-> task_head

When traversing the list, `container_fo` is used to recover the full task structure.

	mini_task_t *task;

	task = mini_container_fo(current,
													 mini_task_t,
													 task_node);

Here:
	
		current = address of task_node
		task		= address of the parent mini_task_t structure

## Task List Example Output

	===== Task double lined list =====
	pid=1, name=init, state=READY, node=0x1d7c584, prev=0x23a8c, next=0x1d7c5bc
	pid=2, name=worker, state=READY, node=0x1d7c5bc, prev=0x1d7c584, next=0x1d7c5f4
	pid=3, name=logger, state=READY, node=0x1d7c5f4, prev=0x1d7c5bc, next=0x23a8c
	==================================

In this output:

	head = 0x23a8c

The list structure is:

	head <-> init <-> worker <-> logger <-> head

The means `mini_task` is now managed using a circular doubly lined list.

## Why mini_list Should Not Depend on mini_task

	`mini_list` is a generic list module.

If Should not know about `mini_task_t`.

The correct dependency direction is:
	mini_task uses mini_list
	mini_list does not use mini_task

So `mini_lsit.c` should only manage list nodes.

Task-specific such as `pid`, `name`, and `state` should be handled in `mini_task.c`.

Correct structure:

	mini_list.c
		- add node
		- delete node
		- check empty
		- show raw addresses

	mini_task.c
		- create task
		- add task to task list
		- convert task_node to mini_task_t using container_of
		- print task information

This keeps the list module reusable.

## What I Learned

Through this module, I learned:

    1. how a circular doubly linked list works
    2. how next and prev pointers connect nodes
    3. how a list head can point to itself when empty
    4. how to add a node to the tail
    5. how to delete a node from the list
    6. how container_of recovers the parent structure
    7. how Linux kernel-style list_head differs from a simple next pointer
    8. how mini_task can use mini_list_head_t instead of a direct next pointer
    9. why mini_list should remain independent from mini_task

## Difference From Real Linux Kernel

This implementation is much simpler than the real Linux kernel list implementation.

This project does not implement:

    list_for_each
    list_for_each_entry
    list_add
    list_add_tail exactly as kernel macro
    list_del_init
    safe iteration
    RCU list
    concurrency protection

The real Linux kernel list implementation is more complete and macro-based.

This module is only a first step to understand the basic idea of `list_head`.

## Summary

In this module, I implemented a simplified Linux kernel-style circular doubly linked list.

The main concept is:

    list head + embedded list node + container_of = kernel-style linked list

Before this module, `mini_task` used a direct `next` pointer.

After this module, `mini_task` uses an embedded `mini_list_head_t` node.

This helped me understand how the Linux kernel can manage many different structures using a common list node.
