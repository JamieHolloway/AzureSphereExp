#include <stdbool.h>
#include <errno.h>
#include <string.h>
#include <time.h>
#include "applibs_versions.h"
#include <applibs/log.h>
#include <applibs/gpio.h>
#include "mt3620_rdb.h"

static const GPIO_Id ledsPins[3][3] = { {MT3620_RDB_LED1_RED, MT3620_RDB_LED1_GREEN, MT3620_RDB_LED1_BLUE}, {MT3620_RDB_LED2_RED, MT3620_RDB_LED2_GREEN, MT3620_RDB_LED2_BLUE}, {MT3620_RDB_LED3_RED, MT3620_RDB_LED3_GREEN, MT3620_RDB_LED3_BLUE} };

static int led1Fd = -1;

int main(void)
{
	Log_Debug("Experiments with Azure Sphere\n");

	int led1GreenFd = GPIO_OpenAsOutput(MT3620_RDB_LED1_GREEN, GPIO_OutputMode_PushPull, GPIO_Value_High);
	if (led1GreenFd < 0) { Log_Debug("Error opening GPIO: %s (%d).\n", strerror(errno), errno); return -1; }

	int led1BlueFd = GPIO_OpenAsOutput(MT3620_RDB_LED1_BLUE, GPIO_OutputMode_PushPull, GPIO_Value_High);
	if (led1BlueFd < 0) { Log_Debug("Error opening GPIO: %s (%d).\n", strerror(errno), errno); return -1; }

	const struct timespec sleepTime = { 2, 0 };
	for (;;)
	{
		GPIO_SetValue(led1BlueFd, GPIO_Value_High);
		GPIO_SetValue(led1GreenFd, GPIO_Value_Low);
		nanosleep(&sleepTime, NULL);
		GPIO_SetValue(led1BlueFd, GPIO_Value_Low);
		GPIO_SetValue(led1GreenFd, GPIO_Value_High);
		nanosleep(&sleepTime, NULL);
	}
}