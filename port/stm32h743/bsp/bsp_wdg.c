/*
 * Copyright (c) 2026 beric-xiong
 * SPDX-License-Identifier: MIT
 *
 * STM32 IWDG initialization using LSI clock (~32 kHz)
 */

#include "bsp_wdg.h"

/* LSI frequency = 32 kHz */
#define LSI_FREQ_HZ     32000UL

/* IWDG prescaler values: 4, 8, 16, 32, 64, 128, 256 */
static const unsigned int  s_prescaler_table[] = { 4, 8, 16, 32, 64, 128, 256 };
static const unsigned int  s_prescaler_reg[]    = {
    IWDG_PRESCALER_4, IWDG_PRESCALER_8, IWDG_PRESCALER_16, IWDG_PRESCALER_32,
    IWDG_PRESCALER_64, IWDG_PRESCALER_128, IWDG_PRESCALER_256
};

IWDG_HandleTypeDef g_iwdg;

void bsp_iwdg_init(unsigned int timeout_ms)
{
    unsigned int reload, best_reload = 0;
    unsigned int  i, best_idx = 0;

    /* Find best prescaler: smallest reload value within 12-bit range */
    for (i = 0; i < 7; i++) {
        reload = (unsigned int)((unsigned long long)timeout_ms * LSI_FREQ_HZ
                    / (s_prescaler_table[i] * 1000));
        if (reload <= 0xFFF) {
            best_reload = reload;
            best_idx = i;
            break;
        }
    }

    /* Timeout too large — use max reload */
    if (best_reload == 0 || best_reload > 0xFFF) {
        best_reload = 0xFFF;
        best_idx = 6;
    }

    /* Timeout too small — use min reload (1) */
    if (best_reload < 1) {
        best_reload = 1;
    }

    g_iwdg.Instance = IWDG1;
    g_iwdg.Init.Prescaler = s_prescaler_reg[best_idx];
    g_iwdg.Init.Reload = best_reload;
    g_iwdg.Init.Window = 0xFFF;  /* no window, use max */
    if (HAL_IWDG_Init(&g_iwdg) != HAL_OK)
    {
        error_handler();
    }
}
