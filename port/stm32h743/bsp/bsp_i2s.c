/*
 * Copyright (c) 2026 beric-xiong
 * SPDX-License-Identifier: MIT
 *
 * STM32 I2S2 initialization (master, Philips standard)
 * PB12=WS, PB13=CK, PB15=SD (AF5)
 */

#include "bsp_i2s.h"

/* APB1 peripheral clock = 100 MHz (PCLK1) */
#define I2S_BASE_CLK_HZ  100000000UL

I2S_HandleTypeDef g_i2s2;

void bsp_i2s2_init(unsigned int sample_rate, unsigned char bits)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    /* Enable GPIOB clock (PB12, PB13, PB15) */
    __HAL_RCC_GPIOB_CLK_ENABLE();
    /* Enable SPI2 clock (I2S2 reuses SPI2) */
    __HAL_RCC_SPI2_CLK_ENABLE();

    /* PB12=WS, PB13=CK, PB15=SD → AF5 (SPI2/I2S2) */
    GPIO_InitStruct.Pin = GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_15;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF5_SPI2;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    /* Configure I2S2 */
    g_i2s2.Instance = SPI2;
    g_i2s2.Init.Mode = I2S_MODE_MASTER_TX;
    g_i2s2.Init.Standard = I2S_STANDARD_PHILIPS;
    g_i2s2.Init.DataFormat = (bits == 16) ? I2S_DATAFORMAT_16B :
                             (bits == 24) ? I2S_DATAFORMAT_24B :
                                            I2S_DATAFORMAT_32B;
    g_i2s2.Init.MCLKOutput = I2S_MCLKOUTPUT_DISABLE;
    g_i2s2.Init.AudioFreq = sample_rate;
    g_i2s2.Init.CPOL = I2S_CPOL_LOW;
    if (HAL_I2S_Init(&g_i2s2) != HAL_OK)
    {
        error_handler();
    }
}
