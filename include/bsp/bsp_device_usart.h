#ifndef __BSP_DEVICE_USART_H__
#define __BSP_DEVICE_USART_H__

#include "stm32f7xx.h"
#include <stdio.h>

// 引脚定义
/*******************************************************/
#define DEVICE_USART USART6
#define DEVICE_USART_CLK_ENABLE() __USART6_CLK_ENABLE();

#define DEVICE_USART_RX_GPIO_PORT GPIOC
#define DEVICE_USART_RX_GPIO_CLK_ENABLE() __HAL_RCC_GPIOC_CLK_ENABLE()
#define DEVICE_USART_RX_PIN GPIO_PIN_7
#define DEVICE_USART_RX_AF GPIO_AF8_USART6

#define DEVICE_USART_TX_GPIO_PORT GPIOC
#define DEVICE_USART_TX_GPIO_CLK_ENABLE() __HAL_RCC_GPIOC_CLK_ENABLE()
#define DEVICE_USART_TX_PIN GPIO_PIN_6
#define DEVICE_USART_TX_AF GPIO_AF8_USART6

#define DEVICE_USART_IRQHandler USART6_IRQHandler
#define DEVICE_USART_IRQ USART6_IRQn
/************************************************************/

// 串口波特率
#define DEVICE_USART_BAUDRATE 9600
void Device_SendString(const char *str);
void Device_USART_Config(void);
extern UART_HandleTypeDef UartDeviceHandle;
#endif /* __BSP_DEVICE_USART_H__ */
