# mini_kernel_simulator

## Project Overview

`mini_kernel_simulator` is a user-space C project for studying ARMv7/Linux kernel concepts.

The goal of this project is not to build a real operating system kernel, but to understand Linux kernel internals by recreating simplified versions of core kernel mechanisms.

This project is built and tested on Raspberry Pi.

## Purpose

While studying the ARMv7/Linux kernel, I realized that reading kernel source code alone was not enough to fully understand the internal structure.

To improve my understanding, I started implementing simplified kernel-like components in user-space C.

This project focuses on learning by building small versions of kernel concepts such as:

* kernel-style logging
* ring buffer
* task/process structure
* linked list
* scheduler simulation
* interrupt handler registration
* character device operation structure

## Current Version

### v0.1 - mini_printk

Implemented:

* log level support
* kernel-style log output
* ring buffer-based log storage
* recent log display

### v0.2 - mini_task

Implemented:

* simplified task structure
* task state management
* task creation using `malloc()`
* task linked list using `next` pointer
* task list display
* task memory release using `free()`
* structure layout experiment using `sizeof()` and `offsetof()`
* array layout and malloc allocation alignment comparison

## Project Structure

```text
mini_kernel_simulator/
├── Makefile
├── README.md
├── docs/
│   ├── 01_mini_printk.md
│   └── 02_mini_task.md
├── include/
│   ├── mini_printk.h
│   └── mini_task.h
└── src/
    ├── main.c
    ├── mini_printk.c
    └── mini_task.c
```

## Build & Run

Build:

```bash
make
```

Run:

```bash
./mini_kernel_simulator
```

Clean:

```bash
make clean
```

## Example Output

```text
[INFO] mini_task initialized
[INFO] mini kernel simulator start
[DEBUG] initializing mini_printk ring buffer
[INFO] task created: pid=1, name=init, state=RUNNING
[INFO] task created: pid=2, name=worker, state=READY
[INFO] task created: pid=3, name=logger, state=SLEEPING
[INFO] task added as head: pid=1, name=init
[INFO] task added to list: pid=2, name=worker
[INFO] task added to list: pid=3, name=logger

===== Task List =====
pid=1, name=init, state=RUNNING, next=0x106e588
pid=2, name=worker, state=READY, next=0x106e5b8
pid=3, name=logger, state=SLEEPING, next=(nil)
=====================
```

The task list shows a simple linked list:

```text
init_task -> worker_task -> logger_task -> NULL
```

## Module Summary

### mini_printk

`mini_printk` is a simplified kernel-style logging module.

It supports:

* log levels
* formatted output
* fixed-size log buffer
* ring buffer overwrite behavior

This module helps me understand the basic concept of `printk()` and kernel log buffers.

### mini_task

`mini_task` is a simplified task management module.

It supports:

* task creation
* task state storage
* linked list connection using `next`
* task list traversal
* memory cleanup

This module helps me understand how task-like structures can be represented and connected in memory.

## Memory Layout Experiment

For `mini_task_t`, I checked the structure size and member offsets.

```text
sizeof(mini_task_t) = 44
offset pid          = 0
offset name         = 4
offset state        = 36
offset next         = 40
```

Array allocation result:

```text
diff 0->1 = 44
diff 1->2 = 44
```

Heap allocation using `malloc()` showed that individually allocated task objects may be placed 48 bytes apart due to heap allocator alignment.

This experiment helped me understand the difference between:

* structure size
* array layout
* heap allocation layout
* allocator alignment

## Documentation

Detailed notes are available in the `docs/` directory.

```text
docs/01_mini_printk.md
docs/02_mini_task.md
```

## Roadmap

* [x] implement mini_printk
* [x] implement ring buffer log storage
* [x] implement mini_task structure
* [x] implement task linked list
* [x] document task memory layout experiment
* [ ] implement simple ready queue
* [ ] implement round-robin scheduler
* [ ] implement interrupt handler table
* [ ] implement character device simulator
* [ ] compare simple `next` pointer list with Linux kernel `list_head`

## Tech Stack

* C
* Makefile
* Git / GitHub
* Raspberry Pi
* Linux user-space environment
* ARMv7/Linux kernel concept study

## Notes

This project is not a real kernel implementation.

It is a learning project that recreates simplified versions of Linux kernel concepts in user-space C.

The purpose is to improve my understanding of kernel source code by implementing small kernel-like modules step by step.

