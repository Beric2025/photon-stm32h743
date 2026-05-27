/*
 * Copyright (c) 2026 beric-xiong
 * SPDX-License-Identifier: MIT
 */

#ifndef _UART_DEV_H_
#define _UART_DEV_H_

#ifdef __cplusplus
 extern "C" {
#endif

typedef struct {
	/* UART controller identifier (e.g. "uart1", "uart2") */
	char *name;

	/**
	 * @brief:  initialize UART peripheral
	 * @privatedata:  UART handle (Uart_Device_T *)
	 * @baudrate: 	  UART baudrate
	 *
	 * Return: 0 on success, -1 on failure
	 */
	int (*init)(void *privatedata, unsigned int baudrate);

	/**
	 * @brief:  send data via UART
	 * @privatedata: UART handle (Uart_Device_T *)
	 * @buff:   	 data buffer to send
	 * @length:      number of bytes to send
	 *
	 * Return: 0 on success, -1 on failure
	 */
	int (*send)(void *privatedata, char *buff, unsigned short length);

	/**
	 * @brief:  receive data from UART
	 * @privatedata:  UART handle (Uart_Device_T *)
	 * @buff:            buffer for received data
	 * @size:            size of the data buffer
	 *
	 * Return: number of bytes received
	 */
	unsigned short (*receive)(void *privatedata, char *buff, unsigned short size);

	/**
	 * @brief:  UART control (ioctl)
	 * @privatedata:  UART handle (Uart_Device_T *)
	 * @flag:            control command
	 *
	 * Return: 0 on success, -1 on failure
	 */
	int (*ioctl)(void *privatedata, unsigned int flag);

	void *private_data;
}Uart_Device_T;

/**
 * @brief:  get UART device structure by name
 * @name:   device name (e.g. "uart1", "uart2")
 *
 * Return: pointer to Uart_Device_T structure, NULL if not found
 */
Uart_Device_T *get_uart_device(char *name);

#ifdef __cplusplus
}
#endif

#endif	/* _UART_DEV_H_ */
