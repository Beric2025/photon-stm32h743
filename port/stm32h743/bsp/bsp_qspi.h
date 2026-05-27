/*
 * Copyright (c) 2026 beric-xiong
 * SPDX-License-Identifier: MIT
 */

#ifndef __BSP_QSPI_H__
#define __BSP_QSPI_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"

extern QSPI_HandleTypeDef g_qspi;

/**
 * @brief:  initialize QSPI Bank1 (single-wire, compatible with NOR Flash)
 *
 * Pins: CLK=PF10, BK1_NCS=PG6, BK1_IO0=PF8, BK1_IO1=PF9,
 *       BK1_IO2=PF7, BK1_IO3=PF6 (AF9)
 */
void bsp_qspi_init(void);

#ifdef __cplusplus
}
#endif

#endif /* __BSP_QSPI_H__ */
