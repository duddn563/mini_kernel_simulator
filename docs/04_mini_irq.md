# 04. mini_irq

## Purpose

This document explains the `mini_irq` module.

The goal of this module is to understand the basic concept of interrupt handler registration and execution by implementing a simplified IRQ handler simulation in user-space C.

This is not a real hardware interrupt system.
It is a simplified learning module for understanding IRQ tables, handler function pointers, IRQ registration, IRQ triggering, and IRQ release.

## Kernel Concept

In an operating system kernel, an interrupt is a signal that tells the CPU that an event needs attention.

For example:

```text
timer interrupt
keyboard interrupt
network interrupt
disk interrupt
```

When an interrupt occurs, the kernel finds the registered interrupt handler and executes it.

In Linux kernel development, a driver can register an interrupt handler using a function such as `request_irq()`.

In this project, I implemented a simplified version called `mini_request_irq()`.

## IRQ Handler Function Pointer

The IRQ handler type is defined as a function pointer.

```c
typedef void (*mini_irq_handler_t)(int irq);
```

This means:

```text
mini_irq_handler_t is a pointer to a function
that receives int irq as an argument
and returns void
```

So functions like this can be registered as IRQ handlers:

```c
static void timer_irq_handler(int irq)
{
    printf("[HANDLER] timer interrupt handler called: irq=%d\n", irq);
}

static void keyboard_irq_handler(int irq)
{
    printf("[HANDLER] keyboard interrupt handler called: irq=%d\n", irq);
}
```

## IRQ Descriptor Structure

Each IRQ entry is represented by `mini_irq_desc_t`.

```c
typedef struct {
    int irq;
    char name[MINI_IRQ_NAME_SIZE];
    mini_irq_handler_t handler;
    int registered;
} mini_irq_desc_t;
```

Each field has the following meaning:

```text
irq        : IRQ number
name       : IRQ name
handler    : registered handler function pointer
registered : whether this IRQ is currently registered
```

## IRQ Table

The IRQ table is a fixed-size array.

```c
static mini_irq_desc_t irq_table[MINI_IRQ_MAX_COUNT];
```

In this project:

```text
MINI_IRQ_MAX_COUNT = 16
```

So IRQ numbers from `0` to `15` can be used.

Each index in the table represents one IRQ descriptor.

```text
irq_table[0]
irq_table[1]
irq_table[2]
...
irq_table[15]
```

## mini_irq_init()

`mini_irq_init()` initializes the IRQ table.

```c
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
```

After initialization:

```text
all IRQ entries are unused
registered = 0
handler = NULL
name = "unused"
```

## mini_request_irq()

`mini_request_irq()` registers an IRQ handler.

```c
int mini_request_irq(int irq, const char *name, mini_irq_handler_t handler);
```

Example:

```c
mini_request_irq(1, "timer", timer_irq_handler);
mini_request_irq(2, "keyboard", keyboard_irq_handler);
```

This stores the handler function address in the IRQ table.

```text
irq_table[1].handler = timer_irq_handler
irq_table[2].handler = keyboard_irq_handler
```

The function checks:

```text
1. whether the IRQ number is valid
2. whether the handler is not NULL
3. whether the IRQ is already registered
```

If all checks pass, the IRQ is registered.

## mini_trigger_irq()

`mini_trigger_irq()` simulates an interrupt event.

```c
int mini_trigger_irq(int irq);
```

Example:

```c
mini_trigger_irq(1);
```

The basic flow is:

```text
1. check if the IRQ number is valid
2. check if the IRQ is registered
3. print IRQ trigger log
4. call the registered handler
```

The actual handler call happens here:

```c
irq_table[irq].handler(irq);
```

This line means:

```text
call the handler function stored in irq_table[irq]
and pass irq as an argument
```

For example:

```text
mini_trigger_irq(1)
-> irq_table[1].handler(1)
-> timer_irq_handler(1)
```

## mini_free_irq()

`mini_free_irq()` releases a registered IRQ.

```c
int mini_free_irq(int irq);
```

When an IRQ is freed:

```text
registered = 0
handler = NULL
name = "unused"
```

After this, triggering the same IRQ will produce a warning because no handler is registered.

## Example Output

```text
[INFO] mini_irq initialized
[INFO] irq registered: irq=1, name=timer
[INFO] irq registered: irq=2, name=keyboard

===== IRQ Table =====
irq=1, name=timer, handler=0x10520, registered=1
irq=2, name=keyboard, handler=0x10560, registered=1
======================

[INFO] irq triggered: irq=1, name=timer
[IRQ] triggered: irq=1, name=timer
[HANDLER] timer interrupt handler called: irq=1
[DEBUG] timer irq handler executed: irq=1

[INFO] irq triggered: irq=2, name=keyboard
[IRQ] triggered: irq=2, name=keyboard
[HANDLER] keyboard interrupt handler called: irq=2
[DEBUG] keyboard irq handler executed: irq=2

[WARN] irq not registered: irq=3
[INFO] irq freed: irq=1, name=timer
[INFO] irq freed: irq=2, name=keyboard
```

This output shows that:

```text
irq=1 triggers timer_irq_handler
irq=2 triggers keyboard_irq_handler
irq=3 is not registered, so a warning is printed
```

## Function Pointer Flow

The key concept of this module is the function pointer.

```text
handler function
↓
registered into IRQ table
↓
IRQ number is triggered
↓
handler function is called through the table
```

Example flow:

```text
timer_irq_handler
↓
mini_request_irq(1, "timer", timer_irq_handler)
↓
irq_table[1].handler = timer_irq_handler
↓
mini_trigger_irq(1)
↓
irq_table[1].handler(1)
↓
timer_irq_handler(1)
```

## What I Learned

Through this module, I learned:

```text
1. how to define a function pointer type
2. how to store handler functions in a table
3. how IRQ registration works conceptually
4. how IRQ triggering calls a registered handler
5. how to check invalid or unregistered IRQ numbers
6. how to release an IRQ handler
7. how mini_printk can be reused for IRQ logs
```

## Difference From Real Linux Kernel

This implementation is much simpler than the real Linux kernel interrupt system.

This project does not implement:

```text
real hardware interrupts
interrupt vector table
CPU exception mode
register saving/restoring
interrupt context
IRQ flags
shared IRQ
top half / bottom half
softirq
tasklet
workqueue
interrupt controller
GIC
```

The real Linux kernel interrupt system is much more complex.

This module is only a first step to understand the basic idea of registering and executing interrupt handlers.

## Summary

In this module, I implemented a simplified IRQ handler simulation.

The main concept is:

```text
IRQ number + handler function pointer + IRQ table = mini_irq
```

The IRQ table stores handler functions.

When an IRQ is triggered, the registered handler is called through the function pointer.

This helped me understand the basic structure behind interrupt handler registration and execution before studying the real Linux kernel interrupt system.

