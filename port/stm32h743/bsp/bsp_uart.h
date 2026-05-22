/*
 * Copyright (c) 2026 beric-xiong
 * SPDX-License-Identifier: MIT
 */

#ifndef _BSP_UART_H_
#define _BSP_UART_H_

#ifdef __cplusplus
 extern "C" {
#endif

#include "main.h"

extern UART_HandleTypeDef g_uart1;
extern UART_HandleTypeDef g_uart4;
extern UART_HandleTypeDef g_uart7;

/**
 * @brief:  initialize UART1 peripheral with specified baud rate
 * @baudrate:   baud rate
 *
 * Return: NULL if not found
 */
void bsp_uart1_init(unsigned int baudrate); 

/**
 * @brief:  initialize UART4 peripheral with specified baud rate
 * @baudrate:   baud rate
 *
 * Return: NULL if not found
 */
void bsp_uart4_init(unsigned int baudrate); 

 /**
 * @brief:  initialize UART7 peripheral with specified baud rate
 * @baudrate:   baud rate
 *
 * Return: NULL if not found
 */
void bsp_uart7_init(unsigned int baudrate); 

#ifdef __cplusplus
}
#endif

#endif /* _BSP_UART_H_ */
