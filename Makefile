CC = gcc
CFLAGS = -Wall -Wextra -Iinclude
TARGET = mini_kernel_simulator

SRC = src/main.c \
			src/mini_printk.c \
			src/mini_task.c

OBJ = $(SRC:.c=.o)

all = $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJ)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@


clean:
	rm -f $(OBJ) $(TARGET)


