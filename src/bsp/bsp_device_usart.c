#include "bsp/bsp_device_usart.h"

UART_HandleTypeDef UartDeviceHandle;
/**
 * @brief  DEVICE_USART GPIO 配置,工作模式配置。115200 8-N-1
 * @param  无
 * @retval 无
 */
void Device_USART_Config(void)
{
    GPIO_InitTypeDef GPIO_InitStruct;

    RCC_PeriphCLKInitTypeDef RCC_PeriphClkInit;

    DEVICE_USART_RX_GPIO_CLK_ENABLE();
    DEVICE_USART_TX_GPIO_CLK_ENABLE();

    /* 配置串口6时钟源*/
    RCC_PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART1;
    RCC_PeriphClkInit.Usart6ClockSelection = RCC_USART6CLKSOURCE_SYSCLK;
    HAL_RCCEx_PeriphCLKConfig(&RCC_PeriphClkInit);
    /* 使能 UART 时钟 */
    DEVICE_USART_CLK_ENABLE();

    /* 配置Tx引脚为复用功能  */
    GPIO_InitStruct.Pin = DEVICE_USART_TX_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
    GPIO_InitStruct.Alternate = DEVICE_USART_TX_AF;
    HAL_GPIO_Init(DEVICE_USART_TX_GPIO_PORT, &GPIO_InitStruct);

    /* 配置Rx引脚为复用功能 */
    GPIO_InitStruct.Pin = DEVICE_USART_RX_PIN;
    GPIO_InitStruct.Alternate = DEVICE_USART_RX_AF;
    HAL_GPIO_Init(DEVICE_USART_RX_GPIO_PORT, &GPIO_InitStruct);

    /* 配置串DEVICE_USART 模式 */
    UartDeviceHandle.Instance = DEVICE_USART;
    UartDeviceHandle.Init.BaudRate = DEVICE_USART_BAUDRATE;
    UartDeviceHandle.Init.WordLength = UART_WORDLENGTH_8B;
    UartDeviceHandle.Init.StopBits = UART_STOPBITS_1;
    UartDeviceHandle.Init.Parity = UART_PARITY_NONE;
    UartDeviceHandle.Init.Mode = UART_MODE_TX_RX;
    UartDeviceHandle.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    UartDeviceHandle.Init.OverSampling = UART_OVERSAMPLING_16;
    UartDeviceHandle.Init.OneBitSampling = UART_ONEBIT_SAMPLING_DISABLED;
    UartDeviceHandle.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
    HAL_UART_Init(&UartDeviceHandle);

    /*串口中断初始化 */
    HAL_NVIC_SetPriority(DEVICE_USART_IRQ, 0, 0);
    HAL_NVIC_EnableIRQ(DEVICE_USART_IRQ);
    /*配置串口接收中断 */
    __HAL_UART_ENABLE_IT(&UartDeviceHandle, UART_IT_RXNE);
}

/*****************  发送字符串 **********************/
void Device_SendString(const char *str)
{
    unsigned int k = 0;
    do
    {
        HAL_UART_Transmit(&UartDeviceHandle, (uint8_t *)(str + k), 1, 1000);
        k++;
    } while (*(str + k) != '\0');
}