## Struct Size and Allocation Alignment

`mini_task` has a size of 44 bytes

However, when tasks are allocated individually using `malloc()`, the returned addresses may differ by 48 bytes due to heap allocator alignment.

when `mini_task` is declared as an array, each element is placed exactly 44 bytes apart because array element are laid out according to `sizeof(mini_task_t)`.

This experiment helped me understand the difference between structure size, array layout, and heap allocator alignment.
