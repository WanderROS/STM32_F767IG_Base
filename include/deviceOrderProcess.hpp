#pragma once
#include "systemConfig.hpp"
/**
 * 设备指令处理
 */

/* 系统配置类 */
extern SystemConfig systemConfig;

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
            injectOrder(ucDeviceRecvBuffer, ulDeviceRecvSize - 1);
            if (systemConfig.getBoolDeviceOutEcho())
            {
                printf(">> 设备输出: ");
            }

            for (int i = 0; i < ulDeviceRecvSize; ++i)
            {
                if (systemConfig.getBoolDeviceOutEcho())
                {
                    printf("%02x ", ucDeviceRecvBuffer[i]);
                }

                HAL_UART_Transmit(&UartWiFiHandle, (uint8_t *)(ucDeviceRecvBuffer + i), 1, 1000);
            }
            if (systemConfig.getBoolDeviceOutEcho())
            {
                printf("\n");
            }
            ucDeviceRecvReady = FALSE;
        }
    }

private:
    void processA0(uint8_t *buffer, int len)
    {
        if (boolCheatA0)
        {
            buffer[12] = projectNo & 0xFF;
            buffer[13] = projectNo / 255;
        }
        buffer[len] = orderCheckSum(buffer, len);
    }
    void process04(uint8_t *buffer, int len)
    {
        if (boolCheatA0)
        {
            buffer[31] = projectNo & 0xFF;
            buffer[32] = projectNo / 255;
        }

        buffer[len] = orderCheckSum(buffer, len);
    }
    void process03(uint8_t *buffer, int len)
    {
        if (boolCheatA0)
        {
            buffer[31] = projectNo & 0xFF;
            buffer[32] = projectNo / 255;
        }
        buffer[len] = orderCheckSum(buffer, len);
    }
    // 注入内容
    void injectOrder(uint8_t *buffer, int len)
    {
        // 设备指令小于 10 没必要注入
        if (len >= 10)
        {
            // 指令
            switch ((unsigned char)buffer[9])
            {
            case 0xA0:
                processA0(buffer, len);
                break;
            case 0x04:
                process04(buffer, len);
                break;
            case 0x03:
                process03(buffer, len);
                break;
            default:
                break;
            }
        }
    }
    /**
     * CRC 校验
     */
    unsigned char orderCheckSum(uint8_t *buffer, int len)
    {
        unsigned char checksum = 0;
        for (int i = 0; i < len - 2; i++)
        {
            checksum += buffer[1 + i];
        }
        checksum = ~checksum + 1;
        return checksum;
    }
    // 项目号
    uint16_t projectNo = 13105;
    // 欺骗项目号,项目号四个品类都是一样的处理逻辑
    bool boolCheatA0 = false;
};