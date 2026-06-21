#mini_timer

## 1. Overview

`mini_timer` is a simplified timer module.

Thie module was created to understand the basic flow of kernel-style timers.

The main concepts are:

- timer registration
- global tick count
- expiration tick
- timer expiration check
- handler execution
- timer disable after execution

The implementation does not use a real hardware timer.

Instead, `mini_timer_tick()` manually increments the tick count.

---

## 2. Timer flow

The basic timer flow is:

		1. Register a timer.
		2. Increment the tick count.
		3. Run the handler when the timer expires.
		3. Disable the timer after the handler has run.

Example:

	mini_timer_add("timer_a", 3, handler)
			-> Schedule timer_a to run after 3 ticks.

	mini_timer_tick()
			-> tick 1

	mini_timer_tick()
			-> tick 2

	mini_timer_tick()
			-> tick 3
			-> timer_a expires
			-> Run the timer_a handler

---

## 3. Header File

The public interface is defined in `include/mini_timer.h`.

		#define MINI_TIMER_MAX_COUNT 8
		#define MINI_TIMER_NAME_SIZE 32

		typedef void (*mini_timer_handler_t)(const char *timer_name);

		void mini_timer_init(void);
		int mini_timer_add(const char *name,
											 int expires_after_ticks,
											 mini_timer_handler_t handler);
		void mini_timer_tick(void);
		void mini_timer_show_all(void);

The timer handler receives the timer name.

		typedef void (mini_timer_handler_t)(const char *timer_name);

This allows each timer handler to know which timer expired.

---

## 4. Timer structure

Each timer is represented by the following structure.

		typedef struct mini_timer {
				int active;
				char name[MINI_TIMER_NAME_SIZE];
				int expires_tick;
				mini_timer_handler_t handler;
		} mini_timer_t;

Each field has a specific role.

		active
				Indicates whether the timer slot is currently used.

		name
				Stores the timer name.

		expires_tick
				Stores the tick count at which the timer  should expire.

		handler
				Stores the function pointer to run when the timer expires.

---

## 5. static Timer Table

The timer module uses 8 static t imer slots.

		static mini_timer_t timer_table[MINI_TIMER_MAX_COUNT];
		static int current_tick_count;

`timer_table` stores all registered timers.

`current_tick_count` stores the current global tick count.

Because they are declared as `static`, they are only accessible inside `mini_timer.c`.

---

# 6. Timer Intialization

The timer module is initialized using:

		void mini_timer_init(void);

This function:

- resets `current_tick_count` to 0
- clears every timer slot
- disables all timers.
- clears timer name.
- removes handlers

Example:

	current_tick_count = 0;

	for (i = 0; i < MINI_TIMER_MAX_COUNT; i++) {
			timer_table[i].active = 0;
			timer_table[i].name[0] = '/0';
			timer_table[i].expires_tick = 0;
			timer_table[i].handler = NULL;
	}

After initialization, all timer slots are empty.

---

## 7. Timer Registration

A timer is registered using:

		int mini_timer_add(const char *name,
											 int expires_after_tick,
											 mini_timer_handler_t handler);

The function checks three invalid cases.

		1. name is NULL
		2. handler is NULL
		3. expires_after_tick is less than or equal to 0

If the input is valid, the function searches for an empty timer slot.

		for (i = 0; i < MINI_TIMER_MAX_COUNT; i++) {
				if (timer_table[i].active == 0) {
						...
				}
		}

When a free slot is found:

		tiemr_table[i].active = 1;
		timer_table[i].expires_tick = 
				current_tick_count + expires_after_ticks;
		timer_table[i].handler = handler;

The important idea is this:

		expires_tick = current_tick_count + expires_after_ticks

For example:

		current_tick_count = 0
		expires_after_tick = 3
		expires_tick 3

The mean the timer will expire when the global tick count reaches 3.

---

## 8. Tick Increment

The tick count is incremented using:

		void mini_timer_tick(void);

The first thing this function does is:

		current_tick_count++;

Then it checks all active timers.

		for (i = 0; i < MINI_TIMER_MAX_COUNT; i++) {
				if (timer_table[i].active == 0) {
						continue;
				}

				if (current_tick_count >= timer_table[i].expires_tick) {
						...
				}
		}

The expiration condition is:

		current_tick_count >= expires_tick

When this condition is true, the timer has expired.

--- 

## 9. Handler Execution

When a timer expires, its handler is executed.

		timer_table[i].handler(timer_table[i].name);

The handler receives the timer name.

Example handler:

		staic void mini_timer_a_handler(const char *timer_name)
		{
				printf("[HANDLER] %s handler run\n", timer_name);

				mini_printk(
						MINI_LOG_INFO,
						"%s handler executed",
						timer_name
				);
		}

This confirms that a timer can execute a registered callback function.

---

## 10. Timer Disable After Execution

After the handler runs, the timer is shdisabled.

		timer_table[i].active = 0;
		timer_table[i].name[0] = '\0';
		tiemr_table[i].expires_tick = 0;
		timer_table[i].handler = NULL;

The means the timer is a one-slot timer.

A one-slot timer runs once and then then becomes inactive.

---

## 11. Example Output

Two timer were registered.

		timer_a -> expires after 3 tick
		timer_b -> expires after 5 tick

Initial timer table:

		slot=0, name=timer_a active = 1, expires_tick = 3, current = 0, remaining = 3
		slot=1, name=timer_b active = 1, expires_tick = 5, current = 3, remaining = 5

After tick 1:
		
		slot=0, name=timer_a, remaining=2
		slot=1, name=timer_b, remaining=4

After tick 2:
		
		slot=0, name=timer_a, remaining=1
		slot=1, name=timer_b, remaining=3

After tick 3:

		timer_a expired
		timer_a handler run

After tick 5:

		timer_b expired
		timer_b handler run


After both timers expire, the timer table becoms empty.

---

## 12. What I Learned

Through `mini_timer`, I learned:

1. how to register a timer
2. how to store a global tick count
3. how to calculate an expiration tick
4. how to check expired timers
5. how to run a handler through a function pointere
6. how to disable a timer after execution
7. how timers can later connect to scheduler and interrupt logic

---

## 13. Relation to kernel concepts

Thie module is related to the following kernel concepts:

- timer interrupt
- scheduler tick
- delayed execution
- timeout handling
- callback function
- one-slot timer
- sleeping task wate-up

In the current version, the timer only runs a handler.

In a future version, this timer can be connected to task state changes.

Example future flow:

		task state = SLEEPING
				->
		timer expires
				->
		task state becoms READY
				->
		scheduler can run the task again

---

## 14. Summury

The main concep of `mini_timer` is:

	current_tick_count + expires_after_tick = expires_tick

The timer is executed when:
	
	current_tick_count >= expires_tick

After execution:

	handler runs
			->
	timer becomes inactive

This module provides a simple foundation for understanding kernel-style timers.

