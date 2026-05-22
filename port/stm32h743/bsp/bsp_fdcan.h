/*
 * Copyright (c) 2026 beric-xiong
 * SPDX-License-Identifier: MIT
 */

#ifndef _BSP_CAN_H_
#define _BSP_CAN_H_

#ifdef __cplusplus
 extern "C" {
#endif

#include "main.h"

#define FDCAN1_RX0_INT_ENABLE 1

extern FDCAN_HandleTypeDef       g_fdcan1;
extern FDCAN_TxHeaderTypeDef     g_fdcan1_tx;
extern FDCAN_RxHeaderTypeDef     g_fdcan1_rx;

/**
 * @brief:  initialize FDCAN1 peripheral
 * @presc:  prescaler value, range 1~512
 * @tsjw:   sync jump width, range 1~128
 * @ntsg1:  time segment 1, range 2~256
 * @ntsg2:  time segment 2, range 2~128
 * @mode:   operating mode -- FDCAN_MODE_NORMAL or FDCAN_MODE_EXTERNAL_LOOPBACK
 *
 * PLL1Q (200 MHz) is the FDCAN clock source.
 *
 * Return: 0 on success, non-zero on failure
 */
unsigned char bsp_fdcan1_init(unsigned short presc, unsigned char tsjw,
    unsigned short ntsg1, unsigned char ntsg2, uint32_t mode);

#ifdef __cplusplus
}
#endif

#endif /* _BSP_CAN_H_ */
