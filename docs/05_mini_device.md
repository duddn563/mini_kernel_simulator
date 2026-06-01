# 05. mini_device

## Purpose

This document explains the `mini_device` module.

The goal of this module is to understand the basic concept of Linux character device driver structure by implementing a simplified device simulation in user-space C.

This is not a real Linux device driver.
It is a simplified learning module for understanding `file_operations`, device open, read, write, and close flow.

## Kernel Concept

In the Linux kernel, a character device driver provides file-like operations.

A user program can interact with a device using operations such as:

```text
open
read
write
close
```

In a real Linux character device driver, these operations are usually connected through `struct file_operations`.

In this project, I implemented a simplified version called `mini_file_operations_t`.

## File Operations Structure

The simplified file operations structure is:

```c
typedef struct {
    int (*open)(void);
    int (*read)(char *buf, int size);
    int (*write)(const char *buf, int size);
    int (*close)(void);
} mini_file_operations_t;
```

Each field is a function pointer.

```text
open  : function pointer for opening the device
read  : function pointer for reading from the device
write : function pointer for writing to the device
close : function pointer for closing the device
```

This structure is similar to the idea of Linux kernel `struct file_operations`.

## Device Buffer

The device has a fixed-size internal buffer.

```c
static char device_buffer[MINI_DEVICE_BUFFER_SIZE];
```

In this project:

```text
MINI_DEVICE_BUFFER_SIZE = 256
```

The device buffer stores data written through the `write()` operation.

## Device Open State

The device open state is tracked using this variable:

```c
static int device_opened = 0;
```

The meaning is:

```text
device_opened = 0 : device is closed
device_opened = 1 : device is opened
```

The device must be opened before read or write operations.

## mini_device_open()

`mini_device_open()` opens the device.

If the device is already opened, it returns an error.

```c
static int mini_device_open(void)
{
    if (device_opened) {
        mini_printk(MINI_LOG_WARN, "device already opened");
        return -1;
    }

    device_opened = 1;
    mini_printk(MINI_LOG_INFO, "device opened");

    return 0;
}
```

## mini_device_write()

`mini_device_write()` writes data into the device buffer.

```c
static int mini_device_write(const char *buf, int size)
```

The function checks:

```text
1. whether the device is opened
2. whether the input buffer is valid
3. whether the size is greater than 0
```

The actual write happens here:

```c
snprintf(device_buffer, MINI_DEVICE_BUFFER_SIZE, "%.*s", size, buf);
```

This stores the given data into `device_buffer`.

## mini_device_read()

`mini_device_read()` reads data from the device buffer.

```c
static int mini_device_read(char *buf, int size)
```

The function checks:

```text
1. whether the device is opened
2. whether the output buffer is valid
3. whether the size is greater than 0
```

The actual read happens here:

```c
snprintf(buf, size, "%s", device_buffer);
```

This copies data from `device_buffer` into the user-provided buffer.

## mini_device_close()

`mini_device_close()` closes the device.

```c
static int mini_device_close(void)
{
    if (!device_opened) {
        mini_printk(MINI_LOG_WARN, "device already closed");
        return -1;
    }

    device_opened = 0;
    mini_printk(MINI_LOG_INFO, "device closed");

    return 0;
}
```

After closing the device, read and write operations are not allowed until it is opened again.

## File Operations Registration

The device operations are connected through `mini_device_fops`.

```c
static mini_file_operations_t mini_device_fops = {
    .open = mini_device_open,
    .read = mini_device_read,
    .write = mini_device_write,
    .close = mini_device_close
};
```

This means:

```text
mini_device_fops.open  -> mini_device_open
mini_device_fops.read  -> mini_device_read
mini_device_fops.write -> mini_device_write
mini_device_fops.close -> mini_device_close
```

## mini_device_get_fops()

`mini_device_get_fops()` returns the address of the file operations structure.

```c
mini_file_operations_t *mini_device_get_fops(void)
{
    return &mini_device_fops;
}
```

In `main.c`, this allows the device to be used like this:

```c
device_fops = mini_device_get_fops();

device_fops->open();
device_fops->write("hello mini device", 17);
device_fops->read(read_buffer, sizeof(read_buffer));
device_fops->close();
```

## Example Output

```text
[INFO] mini_device initialized
[INFO] device opened
[INFO] device write: hello mini device
[INFO] device read: hello mini device
[INFO] device closed
```

This output shows the basic device operation flow:

```text
open -> write -> read -> close
```

## What I Learned

Through this module, I learned:

```text
1. how Linux character device operations are conceptually structured
2. how to use a structure of function pointers
3. how open, read, write, and close operations are connected
4. how to simulate a device buffer in user-space C
5. how to check device open state before read/write
6. how mini_printk can be reused for device operation logs
```

## Difference From Real Linux Kernel

This implementation is much simpler than a real Linux character device driver.

This project does not implement:

```text
real device file
major/minor number
register_chrdev
cdev
copy_to_user
copy_from_user
file pointer
inode
kernel module loading
device node creation
udev
concurrency control
```

The real Linux character device driver structure is much more complex.

This module is only a first step to understand the basic idea of `file_operations`.

## Summary

In this module, I implemented a simplified character device simulation.

The main concept is:

```text
device buffer + file operation function pointers = mini_device
```

This helped me understand how open, read, write, and close operations can be grouped into a file operation structure.

