# 01. mini_printk

## Purpose

This document explains the `mini_printk` module.

The goal of this module is to understand the basic concept of kernel-style logging by implementing a simplified version of `printk()` in user-space C.

This is not a real Linux kernel logging system.
It is a simplified learning module for understanding log levels, formatted output, and ring buffer behavior.

## Kernel Concept

Linux kernel uses `printk()` to print kernel messages.

Unlike normal `printf()`, `printk()` is used inside the kernel and supports different log levels.

Kernel logs are stored in an internal buffer and can be checked later using tools such as `dmesg`.

In this project, I implemented a simplified version called `mini_printk`.

## Log Level

The log level is represented by an enum.

```c
typedef enum {
    MINI_LOG_INFO = 0,
    MINI_LOG_WARN,
    MINI_LOG_ERROR,
    MINI_LOG_DEBUG
} mini_log_level_t;
```

Each log has one of these levels.

```text
INFO  : normal information message
WARN  : warning message
ERROR : error message
DEBUG : debugging message
```

## Log Entry Structure

Each log entry is stored in `mini_log_entry_t`.

```c
typedef struct {
    mini_log_level_t level;
    char message[MINI_LOG_MESSAGE_SIZE];
} mini_log_entry_t;
```

Each log entry contains:

```text
level   : log level
message : log message string
```

## Ring Buffer

The log buffer is a fixed-size array.

```c
static mini_log_entry_t log_buffer[MINI_LOG_BUFFER_SIZE];
static int log_start = 0;
static int log_count = 0;
```

The meaning of each variable is:

```text
log_buffer : fixed-size log storage
log_start  : index of the oldest log
log_count  : number of logs currently stored
```

## Ring Buffer Write Index

Before saving a new log, `mini_printk()` calculates where the new log should be stored.

```c
int write_index = (log_start + log_count) % MINI_LOG_BUFFER_SIZE;
```

The `write_index` decides the position where the new log will be written.

The modulo operator `%` is used to make the index wrap around to the beginning of the buffer.

For example, if the buffer size is 10:

```text
0 -> 1 -> 2 -> 3 -> 4 -> 5 -> 6 -> 7 -> 8 -> 9 -> 0
```

This is why the buffer is called a ring buffer.

## Ring Buffer Overwrite Behavior

When the buffer is not full, new logs are added to the next available position.

When the buffer is full, the oldest log is overwritten.

The important logic is:

```c
if (log_count == MINI_LOG_BUFFER_SIZE) {
    write_index = log_start;
    log_start = (log_start + 1) % MINI_LOG_BUFFER_SIZE;
}
else {
    log_count++;
}
```

This means:

```text
If the buffer is full:
- write the new log at the oldest log position
- move log_start to the next oldest log

If the buffer is not full:
- increase log_count
```

The actual overwrite happens here:

```c
log_buffer[write_index].level = level;

snprintf(log_buffer[write_index].message,
         MINI_LOG_MESSAGE_SIZE,
         "%s",
         temp_message);
```

`write_index` decides where the new log will be stored.

Then `log_buffer[write_index]` is updated with the new log level and message.

## Example of Ring Buffer Behavior

If the buffer size is 5 and logs are inserted like this:

```text
A B C D E
```

The buffer is full.

When a new log `F` is inserted, it overwrites the oldest log `A`.

Physical buffer layout:

```text
F B C D E
```

But the logical order is:

```text
B C D E F
```

This is why `log_start` is moved to the next position after overwriting the oldest log.

## Formatted Output

`mini_printk()` supports formatted output using `stdarg.h`.

```c
va_list args;

va_start(args, format);
vsnprintf(temp_message, sizeof(temp_message), format, args);
va_end(args);
```

This allows `mini_printk()` to be used like this:

```c
mini_printk(MINI_LOG_INFO,
            "task created: pid=%d, name=%s",
            pid,
            name);
```

The formatted string is first stored in `temp_message`.

Then it is copied into the log buffer.

## Example Output

```text
[INFO] mini kernel simulator start
[DEBUG] initializing mini_printk ring buffer
[WARN] this is a warning message
[ERROR] this is an error message
```

When logs are displayed from the buffer:

```text
===== Kernel Log Buffer =====
[WARN] this is a warning message
[ERROR] this is an error message
[INFO] test log 1
[INFO] test log 2
=============================
```

If older logs are missing, it means the ring buffer overwrote them because the buffer was full.

## What I Learned

Through this module, I learned:

```text
1. how to implement kernel-style logging in user-space C
2. how to use log levels
3. how to store logs in a fixed-size buffer
4. how ring buffer overwrite behavior works
5. how to use modulo operation for circular indexing
6. how to use variadic functions with stdarg.h
7. how mini_printk can be reused by other modules
```

## Difference From Real Linux Kernel

This implementation is much simpler than the real Linux kernel `printk()`.

This project does not implement:

```text
kernel log levels like KERN_INFO
dmesg integration
concurrent logging
spinlock protection
console driver output
kernel ring buffer internals
multi-core synchronization
```

The real Linux kernel logging system is much more complex.

This module is only a first step to understand the basic idea of kernel-style logging and ring buffer behavior.

## Summary

In this module, I implemented a simplified kernel-style logging system.

The main concept is:

```text
log level + fixed-size buffer + ring buffer overwrite = mini_printk
```

This module became the base logging system for other modules such as `mini_task` and `mini_scheduler`.

