/*
 * Copyright (c) 2026 beric-xiong
 * SPDX-License-Identifier: MIT
 *
 * can dev
 */

#include <string.h>
#include <stdio.h>
#include "main.h"
#include "can_dev.h"
#include "bsp_fdcan.h"
#include "log_print.h"

#define USE_OS

#ifdef USE_OS
#include "FreeRTOS.h"
#include "task.h"
#endif

#define TAG "can dev:"


/* Buffer definitions */
#define FDCAN1_RX_BUF_SIZE		    16
static uint8_t s_can1_rx_buffer[FDCAN1_RX_BUF_SIZE];

/* Private data structure */
typedef struct {
	FDCAN_HandleTypeDef *fdcan;
    FDCAN_TxHeaderTypeDef *fdcan_tx;
    FDCAN_RxHeaderTypeDef  *fdcan_rx;
	unsigned char (*init)(unsigned short presc, unsigned char tsjw,
	                        unsigned short ntsg1, unsigned char ntsg2,
	                        uint32_t mode);
	uint8_t tx_flag;
	uint8_t *rx_buf;
	uint16_t rx_size;
}Can_Data_T;

/* Function declarations */
static int can_init(void *privatedata);
static int can_send(void *privatedata, unsigned char *data, unsigned short lenght);
static unsigned short can_receive(void *privatedata, unsigned char *data, unsigned short size);

/* FDCAN1 private data */
static Can_Data_T s_fdcan1_data = {
	.fdcan = &g_fdcan1,
    .fdcan_tx = &g_fdcan1_tx,
    .fdcan_rx = &g_fdcan1_rx,
	.init = bsp_fdcan1_init,
	.rx_buf = s_can1_rx_buffer,
	.rx_size = FDCAN1_RX_BUF_SIZE
};

/* FDCAN1 device instance */
static Can_Device_T s_fdcan1_dev = {
	.name = "fdcan1",
	.init = can_init,
	.send = can_send,
	.receive = can_receive,
	.private_data = &s_fdcan1_data,
};

static int can_init(void *privatedata)
{
    if(privatedata == NULL) {
        LOG_PRINT(LOG_OUT_ERROR, "%s Invalid private data is null!\n", TAG);
        return -1;
    }

    Can_Device_T *cav_dev = (Can_Device_T *)privatedata;
    Can_Data_T *cav_data = (Can_Data_T *)cav_dev->private_data;

    cav_data->init(10, 8, 31, 8, FDCAN_MODE_NORMAL);
    LOG_PRINT(LOG_OUT_DEBUG, "%s Invalid successful!\n", TAG);

    return 0;
}
static int can_send(void *privatedata, unsigned char *data, unsigned short lenght)
{
    if(privatedata == NULL) {
        LOG_PRINT(LOG_OUT_ERROR, "%s Send private data is null!\n", TAG);
        return -1;
    }

    Can_Device_T *cav_dev = (Can_Device_T *)privatedata;
    Can_Data_T *cav_data = (Can_Data_T *)cav_dev->private_data;

    cav_data->fdcan_tx->Identifier = 0x12;
    cav_data->fdcan_tx->IdType = FDCAN_STANDARD_ID;
    cav_data->fdcan_tx->TxFrameType = FDCAN_DATA_FRAME;
    cav_data->fdcan_tx->DataLength = lenght;
    cav_data->fdcan_tx->ErrorStateIndicator = FDCAN_ESI_ACTIVE;
    cav_data->fdcan_tx->BitRateSwitch = FDCAN_BRS_OFF;
    cav_data->fdcan_tx->FDFormat = FDCAN_CLASSIC_CAN;
    cav_data->fdcan_tx->TxEventFifoControl = FDCAN_NO_TX_EVENTS;
    cav_data->fdcan_tx->MessageMarker = 0;

    if (HAL_FDCAN_AddMessageToTxFifoQ(cav_data->fdcan, cav_data->fdcan_tx, data) != HAL_OK) {
        LOG_PRINT(LOG_OUT_ERROR, "%s Send failed!\n", TAG);
        return -1;
    }

    LOG_PRINT(LOG_OUT_DEBUG, "%s Send successful!\n", TAG);

    return 0;
}

#if FDCAN1_RX0_INT_ENABLE
/**
 * Interrupt mode: data is buffered in ISR and copied out here.
 * Hardware ISR (HAL_FDCAN_RxFifo0Callback) stores frames into rx_buf,
 * can_receive reads from the buffer. Non-blocking, suitable for
 * high-throughput scenarios where message loss is unacceptable.
 *
 * Return: number of bytes received, 0 if no data or buffer too small
 */
static unsigned short can_receive(void *privatedata, unsigned char *data, unsigned short size)
{
    if(privatedata == NULL) {
        LOG_PRINT(LOG_OUT_ERROR, "%s Receive private data is null!\n", TAG);
        return 0;
    }

    unsigned short ret_size = 0;
    Can_Device_T *cav_dev = (Can_Device_T *)privatedata;
    Can_Data_T *cav_data = (Can_Data_T *)cav_dev->private_data;

    if(cav_data->rx_size){
        /* Return if receive buffer is smaller than the received data */
        if(cav_data->rx_size > size){
            LOG_PRINT(LOG_OUT_WARN, "%s Receive buffer too small! Data size: %d, Buffer size: %d\n", TAG, cav_data->rx_size, size);
            return 0;
        }
#ifdef USE_OS
	    taskENTER_CRITICAL();
#endif        
        memcpy(data, cav_data->rx_buf, cav_data->rx_size);
        ret_size = cav_data->rx_size;
        cav_data->rx_size = 0;
#ifdef USE_OS
	    taskEXIT_CRITICAL();
#endif
	}
    LOG_PRINT(LOG_OUT_DEBUG, "%s Receive successful!\n", TAG);

    return ret_size;
}
#else
/**
 * Polling mode: read directly from hardware RX FIFO each call.
 * Simpler, no ISR or intermediate buffer needed. Suitable for
 * low-traffic use where callers can poll at their own pace.
 *
 * Return: number of bytes received (from DataLength), 0 if no data
 */
static unsigned short can_receive(void *privatedata, unsigned char *data, unsigned short size)
{
    if(privatedata == NULL) {
        LOG_PRINT(LOG_OUT_ERROR, "%s Receive private data is null!\n", TAG);
        return -1;
    }

    Can_Device_T *cav_dev = (Can_Device_T *)privatedata;
    Can_Data_T *cav_data = (Can_Data_T *)cav_dev->private_data;

    if (HAL_FDCAN_GetRxMessage(cav_data->fdcan, FDCAN_RX_FIFO0, cav_data->fdcan_rx, data) != HAL_OK) {
        /* No new message or error */
        LOG_PRINT(LOG_OUT_ERROR, "%s Receive message error!\n", TAG);
        return 0;
    }
    LOG_PRINT(LOG_OUT_DEBUG, "%s Receive message success!\n", TAG);

    return cav_data->fdcan_rx->DataLength>>16;
}
#endif


/**
 * @brief:  get CAN device structure by name
 * @name:   device name (e.g. "fdcan1")
 *
 * Return: pointer to Can_Device_T structure, NULL if not found
 */
Can_Device_T *get_can_device(char *name)
{
    Can_Device_T *cav_dev = NULL;

    if(0 == strcmp(name , "fdcan1"))
      cav_dev = &s_fdcan1_dev;

    return cav_dev;
}


#if FDCAN1_RX0_INT_ENABLE
void FDCAN1_IT0_IRQHandler(void)
{
    /* HAL_FDCAN_IRQHandler(&g_fdcan1); */
}

void HAL_FDCAN_RxFifo0Callback(FDCAN_HandleTypeDef *hfdcan, uint32_t RxFifo0ITs)
{
    /* uint8_t i = 0; */
    uint8_t rxdata[8];
    Can_Data_T *cav_data = NULL;
    uint16_t size = 0;

    if ((RxFifo0ITs & FDCAN_IT_RX_FIFO0_NEW_MESSAGE) != RESET) {
        cav_data = &s_fdcan1_data;
        HAL_FDCAN_GetRxMessage(hfdcan, FDCAN_RX_FIFO0, cav_data->fdcan_rx, rxdata);
        size = cav_data->fdcan_rx->DataLength>>16;
#ifdef USE_OS
		uint32_t task_retval = taskENTER_CRITICAL_FROM_ISR();
#endif
        if((cav_data->rx_size + size) > FDCAN1_RX_BUF_SIZE) {
            cav_data->rx_size = 0;
        }
        memcpy(&cav_data->rx_buf[cav_data->rx_size], rxdata, size);
        cav_data->rx_size += size;
#ifdef USE_OS
		taskEXIT_CRITICAL_FROM_ISR(task_retval);
#endif
        HAL_FDCAN_ActivateNotification(hfdcan, FDCAN_IT_RX_FIFO0_NEW_MESSAGE, 0);
    }
}

#endif

