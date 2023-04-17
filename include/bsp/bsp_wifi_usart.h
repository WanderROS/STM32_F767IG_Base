#ifndef __BSP_WIFI_USART_H__
#define __BSP_WIFI_USART_H__

#include "stm32f7xx.h"
#include <stdio.h>

// 引脚定义
/*******************************************************/
#define UartWiFi_USART USART2
#define UartWiFi_USART_CLK_ENABLE() __USART2_CLK_ENABLE();

#define RCC_PERIPHCLK_UARTx RCC_PERIPHCLK_USART2
#define RCC_UARTxCLKSOURCE_SYSCLK RCC_USART2CLKSOURCE_SYSCLK

#define UartWiFi_USART_RX_GPIO_PORT GPIOD
#define UartWiFi_USART_RX_GPIO_CLK_ENABLE() __HAL_RCC_GPIOD_CLK_ENABLE()
#define UartWiFi_USART_RX_PIN GPIO_PIN_6
#define UartWiFi_USART_RX_AF GPIO_AF7_USART2

#define UartWiFi_USART_TX_GPIO_PORT GPIOD
#define UartWiFi_USART_TX_GPIO_CLK_ENABLE() __HAL_RCC_GPIOD_CLK_ENABLE()
#define UartWiFi_USART_TX_PIN GPIO_PIN_5
#define UartWiFi_USART_TX_AF GPIO_AF7_USART2

#define UartWiFi_USART_IRQHandler USART2_IRQHandler
#define UartWiFi_USART_IRQ USART2_IRQn
/************************************************************/

// 串口波特率
#define UartWiFi_USART_BAUDRATE 9600

void UsartWiFi_SendString(const char *str);
void UartWiFi_USART_Config(void);
extern UART_HandleTypeDef UartWiFiHandle;
#endif /* __BSP_WIFI_USART_H__ */
