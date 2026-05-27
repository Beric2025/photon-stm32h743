/*
 * Copyright (c) 2026 beric-xiong
 * SPDX-License-Identifier: MIT
 *
 * STM32 QSPI Bank1 initialization (single-wire mode)
 * CLK=PF10, BK1_NCS=PG6, BK1_IO0=PF8, BK1_IO1=PF9,
 * BK1_IO2=PF7, BK1_IO3=PF6 (AF9)
 */

#include "bsp_qspi.h"

QSPI_HandleTypeDef g_qspi;

void bsp_qspi_init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    /* Enable QSPI and GPIO clocks */
    __HAL_RCC_QSPI_CLK_ENABLE();
    __HAL_RCC_GPIOF_CLK_ENABLE();
    __HAL_RCC_GPIOG_CLK_ENABLE();

    /* QSPI CLK = PF10 (AF9) */
    GPIO_InitStruct.Pin = GPIO_PIN_10;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF9_QUADSPI;
    HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);

    /* QSPI BK1_NCS = PG6 (AF10) */
    GPIO_InitStruct.Pin = GPIO_PIN_6;
    GPIO_InitStruct.Alternate = GPIO_AF10_QUADSPI;
    HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);

    /* QSPI BK1_IO0 = PF8, IO1 = PF9, IO2 = PF7, IO3 = PF6 (AF10) */
    GPIO_InitStruct.Pin = GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_7 | GPIO_PIN_6;
    GPIO_InitStruct.Alternate = GPIO_AF10_QUADSPI;
    HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);

    /* Configure QSPI */
    g_qspi.Instance = QUADSPI;
    g_qspi.Init.ClockPrescaler = 4;               /* HCLK / 4 */
    g_qspi.Init.FifoThreshold = 1;
    g_qspi.Init.SampleShifting = QSPI_SAMPLE_SHIFTING_NONE;
    g_qspi.Init.FlashSize = 25;                    /* 2^25 = 32 MB */
    g_qspi.Init.ChipSelectHighTime = QSPI_CS_HIGH_TIME_1_CYCLE;
    g_qspi.Init.ClockMode = QSPI_CLOCK_MODE_0;
    g_qspi.Init.FlashID = QSPI_FLASH_ID_1;
    g_qspi.Init.DualFlash = QSPI_DUALFLASH_DISABLE;
    if (HAL_QSPI_Init(&g_qspi) != HAL_OK)
    {
        error_handler();
    }
}
