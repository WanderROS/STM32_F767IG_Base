#pragma once
#include "systemConfig.hpp"
/**
 * WiFi 模组指令处理
 */
/* 系统配置类 */
extern SystemConfig systemConfig;
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
            if (systemConfig.getBoolWiFiOutEcho())
            {
                printf(">> WiFi 输出: ");
            }
            for (int i = 0; i < ulWifiRecvSize; ++i)
            {
                if (systemConfig.getBoolWiFiOutEcho())
                {
                    printf("%02x ", ucWifiRecvBuffer[i]);
                }
                HAL_UART_Transmit(&UartDeviceHandle, (uint8_t *)(ucWifiRecvBuffer + i), 1, 1000);
            }
            if (systemConfig.getBoolWiFiOutEcho())
            {
                printf("\n");
            }
            ucWifiRecvReady = FALSE;
        }
    }
};