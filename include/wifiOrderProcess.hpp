#pragma once
/**
 * WiFi 模组指令处理
 */
extern "C"
{
#include "main.h"
#include "bsp/bsp_device_usart.h"
#include "bsp/bsp_wifi_usart.h"

    extern uint8_t ucWifiRecvBuffer[];
    extern uint16_t ulWifiRecvSize;
    extern uint8_t ucWifiRecvReady;
}

class WiFiOrder
{
public:
    void coreProcess()
    {
        if (ucWifiRecvReady == TRUE)
        {
            printf("串口指令: ");
            for (int i = 0; i < ulWifiRecvSize; ++i)
            {
                printf("%02x ", ucWifiRecvBuffer[i]);
                HAL_UART_Transmit(&UartDeviceHandle, (uint8_t *)(ucWifiRecvBuffer + i), 1, 1000);
            }
            printf("\n");
            ucWifiRecvReady = FALSE;
        }
    }
};