/*
 * Copyright (c) 2026 beric-xiong
 * SPDX-License-Identifier: MIT
 */

#ifndef __BSP_CAP_H__
#define __BSP_CAP_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"

extern TIM_HandleTypeDef g_tim4;

/**
 * @brief:  initialize TIM4 as input capture
 *
 * Channels: CH1=PD12, CH2=PD13 (AF2)
 * Timer clock: 1 MHz (1 µs resolution), ARR=0xFFFF
 */
void bsp_cap4_init(void);

#ifdef __cplusplus
}
#endif

#endif /* __BSP_CAP_H__ */
