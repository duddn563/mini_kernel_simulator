# mini_kernel_simulator

`mini_kernel_simulator` is a user-space C project for studying ARMv7/Linux kernel concepts.

The goal of this project is not to build a real operating system kernel, but to understand kernel internals by recreating versions core kernel mechanisms.

## Current Version

### v0.1 - mini_printk

Implemented:

- log level support
- kernel-style log output
- ring buffer-based log storage
- recent log display

## Build & Run

```bash
make
./mini_kernel_simulator


