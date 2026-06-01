#ifndef MINI_DEVICE_H
#define MINI_DEVICE_H

#define MINI_DEVICE_BUFFER_SIZE 256

typedef struct {
	int (*open)(void);
	int (*read)(char *buf, int size);
	int (*write)(const char *buf, int size);
	int (*close)(void);
} mini_file_operations_t;

void mini_device_init(void);
mini_file_operations_t *mini_device_get_fops(void);

#endif
