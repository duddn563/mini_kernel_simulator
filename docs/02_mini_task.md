# 02. mini_task

## Purpose

This document explains the `mini_task` module.

The goal of this module is to understand how a task-like structure can be represented and connected in C while studying ARMv7/Linux kernel concepts.

This is not a real Linux kernel task implementation.
It is a simplified user-space C implementation for learning purposes.

## Simplified Task Structure

In this project, a task is represented by the `mini_task_t` structure.

```c
typedef struct mini_task {
    int pid;
    char name[MINI_TASK_NAME_SIZE];
    mini_task_state_t state;
    struct mini_task *next;
} mini_task_t;
```

Each field has the following meaning:

```text
pid   : task ID
name  : task name
state : current task state
next  : pointer to the next task
```

The `next` pointer connects one task to another task.

## Task State

```c
typedef enum {
    MINI_TASK_READY = 0,
    MINI_TASK_RUNNING,
    MINI_TASK_SLEEPING,
    MINI_TASK_DONE
} mini_task_state_t;
```

The task state represents the current status of a task.

```text
READY    : waiting to run
RUNNING  : currently running
SLEEPING : waiting or sleeping
DONE     : finished
```

## Linked List Concept

Tasks are connected using the `next` pointer.

```text
init_task -> worker_task -> logger_task -> NULL
```

In this project:

```text
init_task->next   = worker_task
worker_task->next = logger_task
logger_task->next = NULL
```

The last task points to `NULL`, which means the end of the task list.

## Example Output

```text
===== Task List =====
pid=1, name=init, state=RUNNING, next=0x106e588
pid=2, name=worker, state=READY, next=0x106e5b8
pid=3, name=logger, state=SLEEPING, next=(nil)
=====================
```

This output shows that:

```text
init   -> worker
worker -> logger
logger -> NULL
```

The `(nil)` value means that the last task does not point to another task.

## Memory Management

Each task is allocated using `malloc()`.

```c
new_task = (mini_task_t *)malloc(sizeof(mini_task_t));
```

When all tasks are no longer needed, they are released using `free()`.

```c
free(current);
```

This module includes `mini_task_destroy_all()` to release every allocated task in the linked list.

## Structure Layout Experiment

I checked the size and member offsets of `mini_task_t`.

```text
sizeof(mini_task_t) = 44
offset pid          = 0
offset name         = 4
offset state        = 36
offset next         = 40
```

The structure layout is:

```text
offset 0  ~ 3  : pid    4 bytes
offset 4  ~ 35 : name   32 bytes
offset 36 ~ 39 : state  4 bytes
offset 40 ~ 43 : next   4 bytes
```

Total size:

```text
4 + 32 + 4 + 4 = 44 bytes
```

So the actual structure size is 44 bytes.

## Array Layout Experiment

I also tested an array of `mini_task_t`.

```c
mini_task_t tasks[3];
```

The result was:

```text
&tasks[0] = 0x7ed36a80
&tasks[1] = 0x7ed36aac
&tasks[2] = 0x7ed36ad8

diff 0->1 = 44
diff 1->2 = 44
```

This means that array elements are placed exactly according to `sizeof(mini_task_t)`.

```text
tasks[0] -> tasks[1] = 44 bytes
tasks[1] -> tasks[2] = 44 bytes
```

## malloc Allocation Experiment

When tasks were allocated individually using `malloc()`, the task addresses were placed 48 bytes apart in my test.

Although the structure size is 44 bytes, heap allocation may use a larger aligned block size.

```text
structure size        = 44 bytes
malloc block distance = 48 bytes
extra space           = 4 bytes
```

This extra space is not part of `mini_task_t` itself.
It is caused by the heap allocator's alignment policy.

## Array vs malloc

The difference is:

```text
Array:
- elements are placed based on sizeof(mini_task_t)
- in this case, each element is 44 bytes apart

malloc:
- each object is allocated separately
- the allocator may align blocks to 8-byte or 16-byte boundaries
- in this case, allocated task addresses were 48 bytes apart
```

So the important conclusion is:

```text
sizeof(mini_task_t) = 44 bytes
array element distance = 44 bytes
malloc allocation distance = 48 bytes
```

## What I Learned

Through this module, I learned:

```text
1. how to represent a task using a C structure
2. how to connect tasks using a next pointer
3. how a singly linked list works in memory
4. how to allocate and free task structures
5. how to inspect structure size using sizeof()
6. how to inspect member offsets using offsetof()
7. the difference between array layout and malloc allocation layout
8. why heap allocation may add extra spacing due to alignment
```

## Difference From Real Linux Kernel

This project uses a simple `next` pointer.

The real Linux kernel uses more advanced structures and mechanisms, such as:

```text
task_struct
list_head
runqueue
scheduler classes
PID hash table
parent-child task relationships
```

This simplified implementation is only the first step toward understanding how task-like structures can be connected and managed.

## Summary

In this module, I implemented a simplified task linked list using `mini_task_t`.

The main concept is:

```text
task structure + next pointer = linked list
```

I also confirmed that:

```text
mini_task_t itself is 44 bytes,
array elements are 44 bytes apart,
but malloc-allocated task objects may be placed 48 bytes apart because of heap allocator alignment.
```

This helped me understand not only linked lists, but also structure layout and memory allocation behavior in C.

