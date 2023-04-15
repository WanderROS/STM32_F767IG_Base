#include <iostream>
using namespace std;
#include "ArduinoJson.h"
#include "commandParser.hpp"
#include <unordered_map>
// C 库依赖
extern "C"
{
#include "stm32f7xx.h"
#include <stdlib.h>
#include "main.h"
#include <stdio.h>
#include "bsp/bsp_debug_usart.h"
#include "bsp/bsp_led.h"
#include "bsp/bsp_sdram.h"
#include "../lib/FATFS/ff.h"
#include "../lib/FATFS/drivers/fatfs_flash_qspi.h"
#include "../lib/FATFS/ff_gen_drv.h"

    /**
     ******************************************************************************
     *                              定义变量
     ******************************************************************************
     */
    char QSPIPath[4];            /* QSPI flash逻辑驱动器路径 */
    FATFS fs;                    /* FatFs文件系统对象 */
    FIL fnew;                    /* 文件对象 */
    FRESULT res_flash;           /* 文件操作结果 */
    UINT fnum;                   /* 文件成功读写数量 */
    BYTE ReadBuffer[1024] = {0}; /* 读缓冲区 */
    BYTE WriteBuffer[] =         /* 写缓冲区*/
        "测试文件系统2.0\r\n";
    extern FATFS flash_fs;
    extern Diskio_drvTypeDef QSPI_Driver;
}
#define FLASH_WriteAddress 0x00000
#define FLASH_ReadAddress FLASH_WriteAddress
#define FLASH_SectorToErase FLASH_WriteAddress
uint8_t Rx_Buffer[100];

int main()
{

    /* 系统时钟初始化成216 MHz */
    SystemClock_Config();
    /* 初始化串口 */
    DEBUG_USART_Config();
    /* 初始化 LED */
    LED_GPIO_Config();
    cout << "STM32 F767IGT6 调试串口初始化成功！" << endl;
    printf("****** 这是一个SPI FLASH 文件系统实验 ******\r\n");
    // 链接驱动器，创建盘符
    FATFS_LinkDriver(&QSPI_Driver, QSPIPath);
    // 在外部SPI Flash挂载文件系统，文件系统挂载时会对SPI设备初始化
    res_flash = f_mount(&fs, "0:", 1);
    /*----------------------- 格式化测试 ---------------------------*/
    /* 如果没有文件系统就格式化创建创建文件系统 */
    if (res_flash == FR_NO_FILESYSTEM)
    {
        printf("》FLASH还没有文件系统，即将进行格式化...\r\n");
        /* 格式化 */
        res_flash = f_mkfs("0:", 0, 0);

        if (res_flash == FR_OK)
        {
            printf("》FLASH已成功格式化文件系统。\r\n");
            /* 格式化后，先取消挂载 */
            res_flash = f_mount(NULL, "0:", 1);
            /* 重新挂载	*/
            res_flash = f_mount(&fs, "0:", 1);
        }
        else
        {
            printf("《《格式化失败。》》\r\n");
            while (1)
                ;
        }
    }
    else if (res_flash != FR_OK)
    {
        printf("外部Flash挂载文件系统失败。(%d)\r\n", res_flash);
        printf("可能原因:SPI Flash初始化不成功。\r\n");
        while (1)
            ;
    }
    else
    {
        printf("》文件系统挂载成功，可以进行读写测试\r\n");
    }
    /*----------------------- 文件系统测试：写测试 -----------------------------*/
    /* 打开文件，如果文件不存在则创建它 */
    printf("\r\n****** 即将进行文件写入测试... ******\r\n");
    res_flash = f_open(&fnew, "0:Nice.txt", FA_CREATE_ALWAYS | FA_WRITE);
    if (res_flash == FR_OK)
    {
        printf("》打开/创建FatFs读写测试文件.txt文件成功，向文件写入数据。\r\n");
        /* 将指定存储区内容写入到文件内 */
        res_flash = f_write(&fnew, WriteBuffer, sizeof(WriteBuffer), &fnum);
        if (res_flash == FR_OK)
        {
            printf("》文件写入成功，写入字节数据：%d\n", fnum);
            printf("》向文件写入的数据为：\r\n%s\r\n", WriteBuffer);
        }
        else
        {
            printf("！！文件写入失败：(%d)\n", res_flash);
        }
        /* 不再读写，关闭文件 */
        f_close(&fnew);
    }
    else
    {
        printf("！！打开/创建文件失败。\r\n");
    }
    /*------------------- 文件系统测试：读测试 ------------------------------------*/
    printf("****** 即将进行文件读取测试... ******\r\n");
    res_flash = f_open(&fnew, "0:Hello.txt", FA_OPEN_EXISTING | FA_READ);
    if (res_flash == FR_OK)
    {
        printf("》打开文件成功。\r\n");
        res_flash = f_read(&fnew, ReadBuffer, sizeof(ReadBuffer), &fnum);
        if (res_flash == FR_OK)
        {
            printf("》文件读取成功,读到字节数据：%d\r\n", fnum);
            printf("》读取得的文件数据为：\r\n%s \r\n", ReadBuffer);
        }
        else
        {
            printf("！！文件读取失败：(%d)\n", res_flash);
        }
    }
    else
    {
        printf("！！打开文件失败。\r\n");
    }
    /* 不再读写，关闭文件 */
    f_close(&fnew);
    DIR dir;
    static FILINFO fno;
    // 查看根目录下有哪些文件并通过串口输出
    res_flash = f_opendir(&dir, "");
    if (res_flash == FR_OK)
    {
        printf("打开根目录成功！\n");
    }
    for (int i = 0;; i++)
    {
        res_flash = f_readdir(&dir, &fno); // 读取目录项，索引会自动移到下一项
        if (res_flash != FR_OK || fno.fname[0] == 0)
        {
            break; // 说明没有文件
        }
        if (fno.fname[0] == '.')
        {
            continue;
        }
        if (fno.fattrib & AM_DIR) // 判断是文件还是子目录
        {
            printf("(0x%02d)目录", fno.fattrib);
        }
        else
        {
            printf("(0x%02d)文件", fno.fattrib);
        }
        printf("名称为：%s\r\n", (char *)fno.fname);
    }
    /*
     *
     * 不再使用文件系统，取消挂载文件系统
     */
    f_mount(NULL, "0:", 1);

    /*初始化SDRAM模块*/
    SDRAM_Init();
    SDRAM_Test();
    unordered_map<string, int> map;
    map["孙"] = 0;
    map["李"] = 1;
    map["张"] = 2;
    for (auto h : map)
    {
        cout << h.first << "->" << h.second << endl;
    }
    CommandParser cmdParser;
    while (1)
    {
        HAL_Delay(10);
        cmdParser.commandProcess();
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