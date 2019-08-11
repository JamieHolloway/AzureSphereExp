#include <stdbool.h>
#include <errno.h>
#include <string.h>
#include <time.h>
#include "applibs_versions.h"
#include <applibs/log.h>
#include <applibs/gpio.h>
#include "mt3620_rdb.h"
#include "epoll_timerfd_utilities.h"
#include "helpers.h"

bool programRun = true; // set to false to terminate program
static int epollFd = -1;

struct timespec buttonPressCheckPeriod = { 0, 1000000 };
void ButtonPollTimerEventHandler(EventData* eventData);
static EventData buttonPollEventData = { .eventHandler = &ButtonPollTimerEventHandler };
static int buttonGpioFd = -1;
static int buttonPollTimerFd = -1;
GPIO_Value_Type buttonState = GPIO_Value_High;

static int led1GreenFd = -1;
static int led1RedFd = -1;

static int Init(void);
static int CloseDown(void);

int main(void)
{
	Log_Debug("Experiments with Azure Sphere\n");

	if (Init() == -1) return -1;

	while(programRun)
	{
		GPIO_SetValue(led1GreenFd, GPIO_Value_Low); // led1 green is program running

		if (WaitForEventAndCallHandler(epollFd) != 0) { programRun = false; }
	}

	CloseDown();

	return 0;
}

static int Init(void)
{
	Log_Debug("Init and setup\n");

	DebugPrintCurrentlyConnectedWiFiNetwork();

	epollFd = CreateEpollFd(); 
	if (epollFd < 0) { Log_Debug("Failed to create epollFd\n"); return -1; }

	if (!OpenGpioFdAsInput(MT3620_RDB_BUTTON_A, &buttonGpioFd)) { Log_Debug("Error opening button A.\n"); return -1; }

	buttonPollTimerFd = CreateTimerFdAndAddToEpoll(epollFd, &buttonPressCheckPeriod, &buttonPollEventData, EPOLLIN);
	if (buttonPollTimerFd < 0) { Log_Debug("Error creating buttonPollTimerFd.\n"); return -1; }

	led1GreenFd = GPIO_OpenAsOutput(MT3620_RDB_LED1_GREEN, GPIO_OutputMode_PushPull, GPIO_Value_High);
	if (led1GreenFd < 0) { Log_Debug("Error opening GPIO: %s (%d).\n", strerror(errno), errno); return -1; }

	led1RedFd = GPIO_OpenAsOutput(MT3620_RDB_LED1_RED, GPIO_OutputMode_PushPull, GPIO_Value_High);
	if (led1RedFd < 0) { Log_Debug("Error opening GPIO: %s (%d).\n", strerror(errno), errno); return -1; }

	return 0;
}

static int CloseDown()
{
	Log_Debug("exiting program in 10 seconds\n");
	GPIO_SetValue(led1GreenFd, GPIO_Value_High);
	GPIO_SetValue(led1RedFd, GPIO_Value_Low);
	const struct timespec sleepTime = { 10, 0 };
	nanosleep(&sleepTime, NULL);
	GPIO_SetValue(led1RedFd, GPIO_Value_High);

	if (buttonPollTimerFd >= 0) close(buttonPollTimerFd);
	if (led1GreenFd >= 0) close(led1GreenFd);
	if (led1RedFd >= 0) close(led1RedFd);
	if (epollFd >= 0) close(epollFd);
}

void ButtonPollTimerEventHandler(EventData* eventData)
{
	if (ConsumeTimerFdEvent(buttonPollTimerFd) != 0) { Log_Debug("Error in ButtonPollTimerEventHandler.\n"); return; }
	if (IsButtonPressed(buttonGpioFd, &buttonState)) 
	{ 
		Log_Debug("button A pressed -- shut down program\n"); 
		programRun = false;
	}
}