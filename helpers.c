#include <unistd.h>
#include <errno.h>
#include <string.h>
#include "applibs_versions.h"
#include <applibs/gpio.h>
#include <applibs/log.h>
#include <applibs/wificonfig.h>
#include <applibs/networking.h>
#include "epoll_timerfd_utilities.h"
#include "helpers.h"

/// <summary>
///     Show details of the currently connected WiFi network.
/// </summary>
void DebugPrintCurrentlyConnectedWiFiNetwork(void)
{
	WifiConfig_ConnectedNetwork network;
	int result = WifiConfig_GetCurrentNetwork(&network);
	if (result < 0) {
		Log_Debug("INFO: Not currently connected to a WiFi network.\n");
	}
	else {
		Log_Debug("INFO: Currently connected WiFi network: \n");
		Log_Debug("INFO: SSID \"%.*s\", BSSID %02x:%02x:%02x:%02x:%02x:%02x, Frequency %dMHz.\n",
			network.ssidLength, network.ssid, network.bssid[0], network.bssid[1],
			network.bssid[2], network.bssid[3], network.bssid[4], network.bssid[5],
			network.frequencyMHz);
	}
}

/// <summary>
///     Check whether a given button has just been pressed.
/// </summary>
/// <param name="fd">The button file descriptor</param>
/// <param name="oldState">Old state of the button (pressed or released)</param>
/// <returns>true if pressed, false otherwise</returns>
bool IsButtonPressed(int fd, GPIO_Value_Type* oldState)
{
	bool isButtonPressed = false;
	GPIO_Value_Type newState;
	int result = GPIO_GetValue(fd, &newState);
	if (result != 0) { Log_Debug("ERROR: Could not read button GPIO: %s (%d).\n", strerror(errno), errno); }
	else {
		// Button is pressed if it is low and different than last known state.
		isButtonPressed = (newState != *oldState) && (newState == GPIO_Value_Low);
		*oldState = newState;
	}

	return isButtonPressed;
}

/// <summary>
///     Helper function to open a file descriptor for the given GPIO as input mode.
/// </summary>
/// <param name="gpioId">The GPIO to open.</param>
/// <param name="outGpioFd">File descriptor of the opened GPIO.</param>
/// <returns>True if successful, false if an error occurred.</return>
bool OpenGpioFdAsInput(GPIO_Id gpioId, int* outGpioFd)
{
	*outGpioFd = GPIO_OpenAsInput(gpioId);
	if (*outGpioFd < 0) {
		Log_Debug("ERROR: Could not open GPIO '%d': %d (%s).\n", gpioId, errno, strerror(errno));
		return false;
	}

	return true;
}