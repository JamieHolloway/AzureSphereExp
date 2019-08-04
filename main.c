#include <stdbool.h>
#include <errno.h>
#include <string.h>
#include <time.h>
#include "applibs_versions.h"
#include <applibs/log.h>
#include <applibs/gpio.h>
#include "mt3620_rdb.h"
#include "helpers.h"

int led1GreenFd = 0;
int led1BlueFd = 0;
const struct timespec sleepTime = { 2, 0 };

static int Init(void);

int main(void)
{
	Log_Debug("Experiments with Azure Sphere\n");

	if (Init() == -1) return -1;

	for (;;)
	{
		GPIO_SetValue(led1BlueFd, GPIO_Value_High);
		GPIO_SetValue(led1GreenFd, GPIO_Value_Low);
		nanosleep(&sleepTime, NULL);
		GPIO_SetValue(led1BlueFd, GPIO_Value_Low);
		GPIO_SetValue(led1GreenFd, GPIO_Value_High);
		nanosleep(&sleepTime, NULL);
	}
	return 1;
}

static int Init(void)
{
	Log_Debug("Init and setup\n");

	DebugPrintCurrentlyConnectedWiFiNetwork();

	led1GreenFd = GPIO_OpenAsOutput(MT3620_RDB_LED1_GREEN, GPIO_OutputMode_PushPull, GPIO_Value_High);
	if (led1GreenFd < 0) { Log_Debug("Error opening GPIO: %s (%d).\n", strerror(errno), errno); return -1; }

	led1BlueFd = GPIO_OpenAsOutput(MT3620_RDB_LED1_BLUE, GPIO_OutputMode_PushPull, GPIO_Value_High);
	if (led1BlueFd < 0) { Log_Debug("Error opening GPIO: %s (%d).\n", strerror(errno), errno); return -1; }
}