#pragma once
/**
 * 设备指令处理
 */

extern "C"
{
#include "main.h"
#include "bsp/bsp_device_usart.h"
#include "bsp/bsp_wifi_usart.h"
    extern uint8_t ucDeviceRecvBuffer[];
    extern uint16_t ulDeviceRecvSize;
    extern uint8_t ucDeviceRecvReady;
}

class DeviceOrder
{
public:
    void coreProcess()
    {
        if (ucDeviceRecvReady == TRUE)
        {
            printf("设备指令: ");
            for (int i = 0; i < ulDeviceRecvSize; ++i)
            {
                printf("%02x ", ucDeviceRecvBuffer[i]);
                HAL_UART_Transmit(&UartWiFiHandle, (uint8_t *)(ucDeviceRecvBuffer + i), 1, 1000);
            }
            printf("\n");
            ucDeviceRecvReady = FALSE;
        }
    }
};