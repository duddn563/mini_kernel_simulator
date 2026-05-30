# 03. mini_scheduler

## Purpose

This document explains the `mini_scheduler` module.

The goal of this module is to understand the bagic concept of task scheduling by implementing a simplified round-robin scheduler simulation in user-space C.

This is not a real Linux kernel scheduler.
It is a simplified learning module for understanding how a scheduler can select and rotate tasks.

## Kernel Concept

In an operating system kernel, a scheduler decides which task should run next.

A real scheduler considers many factors, such as:

```text
task state
priority
time slice
CPU core
run queue
scheduler policy
```

In this project, I implemented only a very simple version.

The scheduler selects tasks one by using the `next` pointer in the task linked list.

## Simplified Scheduler Structure

The scheduler users two static pointers.

```c
static mini_task_t *scheduler_ready_queue_head = NULL;
static mini_task_t *scheduler_current_task = NULL;
```

```text
scheduler_ready_queue_head	: first task in the ready queue
scheduler_current_task 			: currently selected task
```

In this project, the task linked list is used as a simple ready queue.

## Ready Queue Concept

The task list is connected like this:

```text
init_task -> worker_task -> logger_task -> NULL
```

The scheduler starts from the head task.
```text
scheduler_ready_queue_head = init_task
scheduler_current_task		 = init_task
```

When one task finishes its simulated moves to the next text.

```text
init -> worker -> logger
```

When the scheduler reaches the last task, it returns to the head task.

```text
init -> worker -> logger -> init -> worker -> logger
```

This creates a simple round-robin scheduling behavior.

## mini_scheduler_init()

```c
void mini_scheduler_init(mini_task_t *ready_queue_head)
{
    scheduler_ready_queue_head = ready_queue_head;
    scheduler_current_task = ready_queue_head;
}
```

This function initializes the scheduler by receiving the head of the task list.

The head task becomes the first task to run.

## mini_scheduler_run_once()

`mini_scheduler_run_once()` runs one scheduling step.

The basic flow is:

```text
1. Check if the ready queue is empty
2. Select the current task
3. Change task state to RUNNING
4. Print scheduler log
5. Change task state back to READY
6. Move to the next task
7. If there is no next task, return to the head task
```

The most important part is this logic:

```c
if (scheduler_current_task->next != NULL) {
    scheduler_current_task = scheduler_current_task->next;
} else {
    scheduler_current_task = scheduler_ready_queue_head;
}
```

This code moves the scheduler to the next task.

If the current task is the last task, the scheduler goes back to the first task.

## mini_scheduler_run_rounds()

```c
void mini_scheduler_run_rounds(int rounds)
{
    for (int i = 0; i < rounds; i++) {
        mini_scheduler_run_once();
    }
}
```

This function runs the scheduler multiple times.

For example:

```c
mini_scheduler_run_rounds(6);
```

If there are three tasks, the execution order becomes:

```text
init -> worker -> logger -> init -> worker -> logger
```

## Example Output

```text
===== Scheduler Run =====
[SCHED] running task: pid=1, name=init
[SCHED] running task: pid=2, name=worker
[SCHED] running task: pid=3, name=logger
[SCHED] running task: pid=1, name=init
[SCHED] running task: pid=2, name=worker
[SCHED] running task: pid=3, name=logger
=========================
```

This output shows that the scheduler rotates through the task list in order.

## Task State Transition

During scheduling, the current task state is temporarily changed to `RUNNING`.

```text
READY -> RUNNING -> READY
```

This is a simplified simulation.

In a real operating system, task state changes are more complex and depend on events such as blocking, sleeping, waking up, interrupt handling, and time slice expiration.

## What I Learned

Through this module, I learned:

```text
1. how a scheduler can track the current task
2. how a task list can be used as a simple ready queue
3. how round-robin scheduling rotates through tasks
4. how to move from one task to the next using the next pointer
5. how to return to the head task after reaching the last task
6. how task state can change during simulated scheduling
7. how mini_printk can be reused for scheduler logging
```

## Difference From Real Linux Kernel

This implementation is much simpler than the real Linux scheduler.

This project does not implement:

```text
priority
time slice
context switching
CPU registers
process stack switching
scheduler classes
real runqueue
interrupt-based preemption
multi-core scheduling
```

The real Linux kernel scheduler is much more complex.

This module is only a first step to understand the basic idea of selecting and rotating tasks.

## Summary

In this module, I implemented a simplified round-robin scheduler simulation.

The main concept is:

```text
task linked list + current task pointer = simple round-robin scheduler
```

The scheduler follows the `next` pointer to select the next task.

When it reaches the last task, it returns to the head task and continues running.

This helped me understand the basic structure of scheduler flow before studying the real Linux kernel scheduler.

