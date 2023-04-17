#include "bsp/bsp_wifi_usart.h"

UART_HandleTypeDef UartWiFiHandle;
/**
 * @brief
 * @param  无
 * @retval 无
 */
void UartWiFi_USART_Config(void)
{
    GPIO_InitTypeDef GPIO_InitStruct;

    RCC_PeriphCLKInitTypeDef RCC_PeriphClkInit;

    UartWiFi_USART_RX_GPIO_CLK_ENABLE();
    UartWiFi_USART_TX_GPIO_CLK_ENABLE();

    /* 配置串口2时钟源*/
    RCC_PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART2;
    RCC_PeriphClkInit.Usart2ClockSelection = RCC_USART2CLKSOURCE_SYSCLK;
    HAL_RCCEx_PeriphCLKConfig(&RCC_PeriphClkInit);
    /* 使能 UART 时钟 */
    UartWiFi_USART_CLK_ENABLE();

    /**USART2 GPIO Configuration
    PD5    ------> USART2_TX
    PD6    ------> USART2_RX
    */
    /* 配置Tx引脚为复用功能  */
    GPIO_InitStruct.Pin = UartWiFi_USART_TX_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
    GPIO_InitStruct.Alternate = UartWiFi_USART_TX_AF;
    HAL_GPIO_Init(UartWiFi_USART_TX_GPIO_PORT, &GPIO_InitStruct);

    /* 配置Rx引脚为复用功能 */
    GPIO_InitStruct.Pin = UartWiFi_USART_RX_PIN;
    GPIO_InitStruct.Alternate = UartWiFi_USART_RX_AF;
    HAL_GPIO_Init(UartWiFi_USART_RX_GPIO_PORT, &GPIO_InitStruct);

    UartWiFiHandle.Instance = UartWiFi_USART;
    UartWiFiHandle.Init.BaudRate = UartWiFi_USART_BAUDRATE;
    UartWiFiHandle.Init.WordLength = UART_WORDLENGTH_8B;
    UartWiFiHandle.Init.StopBits = UART_STOPBITS_1;
    UartWiFiHandle.Init.Parity = UART_PARITY_NONE;
    UartWiFiHandle.Init.Mode = UART_MODE_TX_RX;
    UartWiFiHandle.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    UartWiFiHandle.Init.OverSampling = UART_OVERSAMPLING_16;
    UartWiFiHandle.Init.OneBitSampling = UART_ONEBIT_SAMPLING_DISABLED;
    UartWiFiHandle.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
    HAL_UART_Init(&UartWiFiHandle);

    /*串口中断初始化 */
    HAL_NVIC_SetPriority(UartWiFi_USART_IRQ, 0, 0);
    HAL_NVIC_EnableIRQ(UartWiFi_USART_IRQ);
    /*配置串口接收中断 */
    __HAL_UART_ENABLE_IT(&UartWiFiHandle, UART_IT_RXNE);
}

/*****************  发送字符串 **********************/
void UsartWiFi_SendString(const char *str)
{
    unsigned int k = 0;
    do
    {
        HAL_UART_Transmit(&UartWiFiHandle, (uint8_t *)(str + k), 1, 1000);
        k++;
    } while (*(str + k) != '\0');
}