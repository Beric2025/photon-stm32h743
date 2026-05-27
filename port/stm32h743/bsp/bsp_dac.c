/*
 * Copyright (c) 2026 beric-xiong
 * SPDX-License-Identifier: MIT
 *
 * STM32 DAC1 initialization (2-channel, 12-bit)
 * CH1 = PA4, CH2 = PA5
 */

#include "bsp_dac.h"

DAC_HandleTypeDef g_dac1;

void bsp_dac1_init(void)
{
    DAC_ChannelConfTypeDef sConfig = {0};
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    /* Enable GPIOA clock (PA4, PA5) */
    __HAL_RCC_GPIOA_CLK_ENABLE();
    /* Enable DAC1 clock */
    __HAL_RCC_DAC12_CLK_ENABLE();

    /* PA4 → DAC1_OUT1, PA5 → DAC1_OUT2 */
    GPIO_InitStruct.Pin = GPIO_PIN_4 | GPIO_PIN_5;
    GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    g_dac1.Instance = DAC1;
    if (HAL_DAC_Init(&g_dac1) != HAL_OK)
    {
        error_handler();
    }

    /* Configure both channels: 12-bit, no trigger, output buffer enabled */
    sConfig.DAC_Trigger = DAC_TRIGGER_NONE;
    sConfig.DAC_OutputBuffer = DAC_OUTPUTBUFFER_ENABLE;

    HAL_DAC_ConfigChannel(&g_dac1, &sConfig, DAC_CHANNEL_1);
    HAL_DAC_ConfigChannel(&g_dac1, &sConfig, DAC_CHANNEL_2);
}
