/*
 * Copyright (c) 2026 beric-xiong
 * SPDX-License-Identifier: MIT
 *
 * UART management
 */

#include <string.h>
#include <stdio.h>
#include "uart_dev.h"
#include "bsp_uart.h"
#include "main.h"
#include "log_print.h"

#ifdef USE_OS
#include "FreeRTOS.h"
#include "task.h"
#endif

#define TAG "uart dev:"

/* UART DMA buffer management definitions */
#define UART_DMA_ALIGN_SIZE(size)     (((size + 31) & ~31))

/* DMA buffer attribute definitions */
#define UART_DMA_BUFFER_CACHEABLE     0

#define UART7_RX_BUF_SIZE             100

static uint8_t s_uart7_dma_rx_buffer[UART7_RX_BUF_SIZE] __attribute__((aligned(32))); 
static uint8_t s_uart7_rx_buffer[UART7_RX_BUF_SIZE];


/* Private data structure */
typedef struct {
	UART_HandleTypeDef *uart;
	void (*init)(unsigned int baudrate);
	uint8_t dma_on;
	uint8_t tx_flag;
	uint8_t *dma_buffer;
	uint16_t dma_buffer_size;
	uint8_t *rx_buffer;
	uint16_t rx_length;
}Uart_Data_T;

/* Function declarations */
static int uart_init(void *privatedata, unsigned int baudrate);
static int uart_send(void *privatedata, char *buff, unsigned short length);
static unsigned short uart_receive(void *privatedata, char *buff, unsigned short size);
static int uart_ioctl(void *privatedata, unsigned int flag);

/* Define private data structure and initialize */
static Uart_Data_T s_uart1_data = {
	.uart = &g_uart1,
	.init = bsp_uart1_init,
	.dma_on = 0
};
static Uart_Data_T s_uart4_data = {
	.uart = &g_uart4,
	.init = bsp_uart4_init,
	.dma_on = 0
};
static Uart_Data_T s_uart7_data = {
	.uart = &g_uart7,
	.init = bsp_uart7_init,
	.dma_on = 1,
	.dma_buffer = s_uart7_dma_rx_buffer,
	.dma_buffer_size = UART7_RX_BUF_SIZE,
	.rx_buffer = s_uart7_rx_buffer,
	.rx_length = 0
};

/* Device structure */
static Uart_Device_T s_uart1_dev = {
	.name = "uart1",
	.init = uart_init,
	.send = uart_send,
	.receive = uart_receive,
	.private_data = &s_uart1_data,
};
static Uart_Device_T s_uart4_dev = {
	.name = "uart4",
	.init = uart_init,
	.send = uart_send,
	.receive = uart_receive,
	.ioctl = uart_ioctl,
	.private_data = &s_uart4_data,
};
static Uart_Device_T s_uart7_dev = {
	.name = "uart7",
	.init = uart_init,
	.send = uart_send,
	.receive = uart_receive,
	.private_data = &s_uart7_data,
}; 


static int uart_init(void *privatedata, unsigned int baudrate)
{
	if(privatedata == NULL) {
		LOG_PRINT(LOG_OUT_ERROR, "%s UART init private data is null!\n", TAG);
		return -1;
	}

	Uart_Device_T *uart_dev = (Uart_Device_T *)privatedata;
	Uart_Data_T *uart_data = (Uart_Data_T *)uart_dev->private_data;

	if (uart_data == NULL)	{
		LOG_PRINT(LOG_OUT_ERROR, "%s UART data is null!\n", TAG);
		return -1;
	}

	uart_data->init(baudrate);

	if(uart_data->dma_on) {
		HAL_UARTEx_ReceiveToIdle_DMA(uart_data->uart, uart_data->dma_buffer, uart_data->dma_buffer_size);
	}

	LOG_PRINT(LOG_OUT_DEBUG, "%s UART init successful!\n", TAG);

	return 0;
}

static int uart_send(void *privatedata, char *buff, unsigned short length)
{
	if(privatedata == NULL) {
		LOG_PRINT(LOG_OUT_ERROR, "%s UART send private data is null!\n", TAG);
		return -1;
	}

	Uart_Device_T *uart_dev = (Uart_Device_T *)privatedata;
	Uart_Data_T *uart_data = (Uart_Data_T *)uart_dev->private_data;

	if(uart_data->dma_on) {
		SCB_CleanDCache_by_Addr((unsigned int *)buff, UART_DMA_ALIGN_SIZE(length));
		HAL_UART_Transmit_DMA(uart_data->uart, (uint8_t *)buff, length);
		while(__HAL_UART_GET_FLAG(uart_data->uart,UART_FLAG_TC) == RESET) {}

		LOG_PRINT(LOG_OUT_DEBUG, "%s UART send successful!\n", TAG);
	}
	else {
		LOG_PRINT(LOG_OUT_DEBUG, "%s UART send successful!\n", TAG);
		return HAL_UART_Transmit(uart_data->uart, (uint8_t *)buff, length, 1000);
	}

	return 0;
	
}

static unsigned short uart_receive(void *privatedata, char *buff, unsigned short size)
{
	if(privatedata == NULL) {
		LOG_PRINT(LOG_OUT_ERROR, "%s UART receive private data is null!\n", TAG);
		return 0;
	}

	uint16_t ret_size = 0;
	Uart_Device_T *uart_dev = (Uart_Device_T *)privatedata;
	Uart_Data_T *uart_data = (Uart_Data_T *)uart_dev->private_data;

	if(uart_data == NULL) {
		LOG_PRINT(LOG_OUT_ERROR, "%s UART data is null!\n", TAG);
		return 0;
	}

	if(uart_data->rx_length){
		if(uart_data->rx_length > size) {
			LOG_PRINT(LOG_OUT_ERROR, "%s UART receive buffer size is too small!\n", TAG);
			return 0;
		}
#ifdef USE_OS
	taskENTER_CRITICAL();
#endif
		memcpy(buff, uart_data->rx_buffer, uart_data->rx_length);
		ret_size = uart_data->rx_length;
		memset(uart_data->rx_buffer, 0, uart_data->rx_length);
		uart_data->rx_length = 0;

#ifdef USE_OS
	taskEXIT_CRITICAL();
#endif
	}
	LOG_PRINT(LOG_OUT_DEBUG, "%s UART receive successful!\n", TAG);
	return ret_size;
}
static int uart_ioctl(void *privatedata, unsigned int flag)
{
	if(privatedata == NULL) {
		LOG_PRINT(LOG_OUT_ERROR, "%s UART ioctl private data is null!\n", TAG);
		return -1;
	}

	privatedata = privatedata;
	flag = flag;

	GPIO_InitTypeDef GPIO_InitStruct = {0};
	
	GPIO_InitStruct.Pin = GPIO_PIN_0;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	GPIO_InitStruct.Alternate = GPIO_AF8_UART4;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	LOG_PRINT(LOG_OUT_DEBUG, "%s UART ioctl successful!\n", TAG);
	return 0;
}

/* ====================================================================
 * uart_dev_on_event — single entry point from BSP HAL callbacks
 *
 * BSP layer calls this with a void* handle (the HAL UART_HandleTypeDef
 * cast to void*).  We cast it back internally to locate the device and
 * dispatch to the correct event handler.
 *
 * Runs in ISR context — keep it short and non-blocking.
 * ==================================================================== */
void uart_dev_on_event(void *huart_void, Uart_Event_T event, unsigned short size)
{
	UART_HandleTypeDef *huart = (UART_HandleTypeDef *)huart_void;
	Uart_Data_T *uart_data = NULL;

	/* Locate the device by matching the HAL handle */
	if (huart == s_uart1_data.uart) {
		uart_data = &s_uart1_data;
	} else if (huart == s_uart4_data.uart) {
		uart_data = &s_uart4_data;
	} else if (huart == s_uart7_data.uart) {
		uart_data = &s_uart7_data;
	} else {
		return;   /* unknown UART — not registered with this layer */
	}

	switch (event) {

		case UART_EVENT_RX_IDLE:
		{
	#ifdef USE_OS
			unsigned int task_retval = taskENTER_CRITICAL_FROM_ISR();
	#endif
			/* Buffer overflow guard — restart from 0 if frame exceeds buffer */
			if ((uart_data->rx_length + size) > uart_data->dma_buffer_size)
				uart_data->rx_length = 0;

			if (uart_data->dma_on) {
				/* Invalidate cache before copying to ensure latest data */
				SCB_InvalidateDCache_by_Addr((unsigned int*)uart_data->dma_buffer, UART_DMA_ALIGN_SIZE(size));
			}

			memcpy(uart_data->rx_buffer + uart_data->rx_length,
				uart_data->dma_buffer, size);
			uart_data->rx_length += size;

			memset(uart_data->dma_buffer, 0, uart_data->dma_buffer_size);
	#ifdef USE_OS
			taskEXIT_CRITICAL_FROM_ISR(task_retval);
	#endif
			/* Re-arm DMA reception for the next frame */
			if (uart_data->dma_on) {
				HAL_UARTEx_ReceiveToIdle_DMA(uart_data->uart,
					uart_data->dma_buffer, uart_data->dma_buffer_size);
			}
			break;
		}

		case UART_EVENT_TX_COMPLETE:
	#ifdef USE_OS
		{
			unsigned int task_retval = taskENTER_CRITICAL_FROM_ISR();
			uart_data->tx_flag = 1;
			taskEXIT_CRITICAL_FROM_ISR(task_retval);
		}
	#else
			uart_data->tx_flag = 1;
	#endif
			break;

		case UART_EVENT_TX_HALF:
			/* Reserved for future use (e.g. double-buffered TX) */
			break;

		case UART_EVENT_ERROR:
			/* On error, re-arm DMA so reception doesn't stall permanently */
			if (uart_data->dma_on) {
				HAL_UARTEx_ReceiveToIdle_DMA(uart_data->uart,
					uart_data->dma_buffer, uart_data->dma_buffer_size);
			}
			break;

		default:
			break;
	}
}

/**
 * get_uart_device - Get corresponding device structure
 * @name: device name
 *
 * Return: pointer to device structure
 */
Uart_Device_T *get_uart_device(char *name)
{
	Uart_Device_T *uart_dev = NULL;

	if(0 == strcmp(name , "uart1"))
		uart_dev = &s_uart1_dev;
	
	if(0 == strcmp(name , "uart4"))
		uart_dev = &s_uart4_dev;
	
	if(0 == strcmp(name , "uart7"))
		uart_dev = &s_uart7_dev;

	return uart_dev;

}
