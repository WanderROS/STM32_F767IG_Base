#include <iostream>
using namespace std;

// C 库依赖
extern "C"
{
#include "stm32f7xx.h"
#include <stdlib.h>
#include "main.h"
#include <stdio.h>
#include "bsp/bsp_debug_usart.h"
#include "bsp/bsp_led.h"
#include "bsp/bsp_qspi_flash.h"
#include "bsp/bsp_sdram.h"
}
#define FLASH_WriteAddress 0x00000
#define FLASH_ReadAddress FLASH_WriteAddress
#define FLASH_SectorToErase FLASH_WriteAddress
uint8_t Rx_Buffer[100];
int main()
{
    // 读取的 Flash ID 存储位置
    __IO uint32_t DeviceID = 0;
    __IO uint32_t FlashID = 0;

    /* 系统时钟初始化成216 MHz */
    SystemClock_Config();
    /* 初始化串口 */
    DEBUG_USART_Config();
    /* 初始化 LED */
    LED_GPIO_Config();
    cout << "STM32 F767IGT6 调试串口初始化成功！" << endl;
    /* 16M 串行 flash W25Q128 初始化 */
    QSPI_FLASH_Init();
    /* 获取 Flash Device ID */
    DeviceID = QSPI_FLASH_ReadDeviceID();
    HAL_Delay(200);

    /* 获取 SPI Flash ID */
    FlashID = QSPI_FLASH_ReadID();
    printf("\r\nFlashID is 0x%X,  Manufacturer Device ID is 0x%X\r\n", FlashID, DeviceID);
    /* 检验 SPI Flash ID */
    if (FlashID == sFLASH_ID)
    {
        printf("\r\n检测到QSPI FLASH W25Q128 !\r\n");
        /* 将刚刚写入的数据读出来放到接收缓冲区中 */
        BSP_QSPI_Read(Rx_Buffer, FLASH_ReadAddress, 100);
        printf("\r\n读出的数据为：\r\n%s", Rx_Buffer);
    }
    else
    {
        printf("\r\n获取不到 W25Q128 ID!\n\r");
    }
    /*初始化SDRAM模块*/
    SDRAM_Init();
    SDRAM_Test();
    while (1)
    {
        HAL_Delay(1000);
    }
    return 0;
}

/**
 * @brief  System Clock 配置
 *         system Clock 配置如下 :
 *            System Clock source            = PLL (HSE)
 *            SYSCLK(Hz)                     = 216000000
 *            HCLK(Hz)                       = 216000000
 *            AHB Prescaler                  = 1
 *            APB1 Prescaler                 = 4
 *            APB2 Prescaler                 = 2
 *            HSE Frequency(Hz)              = 25000000
 *            PLL_M                          = 25
 *            PLL_N                          = 432
 *            PLL_P                          = 2
 *            PLL_Q                          = 9
 *            VDD(V)                         = 3.3
 *            Main regulator output voltage  = Scale1 mode
 *            Flash Latency(WS)              = 7
 */
void SystemClock_Config(void)
{
    RCC_ClkInitTypeDef RCC_ClkInitStruct;
    RCC_OscInitTypeDef RCC_OscInitStruct;
    HAL_StatusTypeDef ret = HAL_OK;

    /* 使能HSE，配置HSE为PLL的时钟源，配置PLL的各种分频因子M N P Q
     * PLLCLK = HSE/M*N/P = 25M / 25 *432 / 2 = 216M
     */
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
    RCC_OscInitStruct.HSEState = RCC_HSE_ON;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
    RCC_OscInitStruct.PLL.PLLM = 25;
    RCC_OscInitStruct.PLL.PLLN = 432;
    RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
    RCC_OscInitStruct.PLL.PLLQ = 9;

    ret = HAL_RCC_OscConfig(&RCC_OscInitStruct);
    if (ret != HAL_OK)
    {
        while (1)
        {
            ;
        }
    }

    /* 激活 OverDrive 模式以达到216M频率  */
    ret = HAL_PWREx_EnableOverDrive();
    if (ret != HAL_OK)
    {
        while (1)
        {
            ;
        }
    }

    /* 选择PLLCLK作为SYSCLK，并配置 HCLK, PCLK1 and PCLK2 的时钟分频因子
     * SYSCLK = PLLCLK     = 216M
     * HCLK   = SYSCLK / 1 = 216M
     * PCLK2  = SYSCLK / 2 = 108M
     * PCLK1  = SYSCLK / 4 = 54M
     */
    RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

    ret = HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_7);
    if (ret != HAL_OK)
    {
        while (1)
        {
            ;
        }
    }
}