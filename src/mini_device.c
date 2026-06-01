#include "mini_device.h"
#include "mini_printk.h"

#include <stdio.h>
#include <string.h>

static char device_buffer[MINI_DEVICE_BUFFER_SIZE];
static int device_opened = 0;

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

static int mini_device_read(char *buf, int size)
{
	if (!device_opened) {
		mini_printk(MINI_LOG_WARN, "cannot read: device is not opened");
		return -1;
	}

	if (buf == NULL || size <= 0) {
		mini_printk(MINI_LOG_ERROR, "invalid read buffer");
		return -1;
	}

	snprintf(buf, size, "%s", device_buffer);

	mini_printk(MINI_LOG_INFO, "device read: %s", buf);

	return strlen(buf);
}

static int mini_device_write(const char *buf, int size) 
{
	if (!device_opened) {
		mini_printk(MINI_LOG_WARN, "cannot write: device is not opened");
		return -1;
	}

	if (buf == NULL || size <= 0) {
		mini_printk(MINI_LOG_ERROR, "invalid write buffer");
		return -1;
	}

	snprintf(device_buffer, MINI_DEVICE_BUFFER_SIZE, "%.*s", size, buf);

	mini_printk(MINI_LOG_INFO, "device write: %s", device_buffer);

	return strlen(device_buffer);
}

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

static mini_file_operations_t mini_device_fops = {
	.open = mini_device_open,
	.read = mini_device_read,
	.write = mini_device_write,
	.close = mini_device_close
};

void mini_device_init(void)
{
	memset(device_buffer, 0, sizeof(device_buffer));
	device_opened = 0;

	mini_printk(MINI_LOG_INFO, "mini_device initialized");
}

mini_file_operations_t* mini_device_get_fops(void)
{
	return &mini_device_fops;
}

