#pragma once
void DebugPrintCurrentlyConnectedWiFiNetwork(void);
bool IsButtonPressed(int fd, GPIO_Value_Type* oldState);
bool OpenGpioFdAsInput(GPIO_Id gpioId, int* outGpioFd);