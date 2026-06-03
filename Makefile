CC = gcc
CFLAGS = -Wall -Wextra -Iinclude
TARGET = mini_kernel_simulator

SRC = src/main.c \
			src/mini_printk.c \
			src/mini_task.c	\
			src/mini_scheduler.c	\
			src/mini_irq.c	\
			src/mini_device.c	\
			src/mini_list.c

OBJ = $(SRC:.c=.o)

all = $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJ)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@


clean:
	rm -f $(OBJ) $(TARGET)


