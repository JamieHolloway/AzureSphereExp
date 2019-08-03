#include <stdbool.h>
#include <errno.h>
#include <string.h>
#include <time.h>

#include <applibs/log.h>
#include <applibs/gpio.h>

int main(void)
{
	Log_Debug("Experiments with Azure Sphere\n");

	int fd = GPIO_OpenAsOutput(9, GPIO_OutputMode_PushPull, GPIO_Value_High);
	if (fd < 0) {
		Log_Debug("Error opening GPIO: %s (%d).\n", strerror(errno), errno);
		return -1;
	}

	const struct timespec sleepTime = { 4, 0 };
	for (;;)
	{
		GPIO_SetValue(fd, GPIO_Value_Low);
		nanosleep(&sleepTime, NULL);
		GPIO_SetValue(fd, GPIO_Value_High);
		nanosleep(&sleepTime, NULL);
	}
}