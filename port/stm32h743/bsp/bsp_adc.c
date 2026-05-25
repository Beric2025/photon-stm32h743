/*
 * Copyright (c) 2026 beric-xiong
 * SPDX-License-Identifier: MIT
 *
 * ADC1 initialization with DMA (channels 10, 11, 17)
 */

#include "bsp_adc.h"


ADC_HandleTypeDef g_adc1;
DMA_HandleTypeDef g_hdma_adc1;

void bsp_adc1_init(void)
{

    ADC_MultiModeTypeDef multimode = {0};
    ADC_ChannelConfTypeDef sConfig = {0};

    /* DMA2_Stream6 interrupt configuration */
    HAL_NVIC_SetPriority(DMA2_Stream6_IRQn, 5, 0);
    HAL_NVIC_EnableIRQ(DMA2_Stream6_IRQn);

    /* ADC common configuration */
    g_adc1.Instance = ADC1;
    g_adc1.Init.ClockPrescaler = ADC_CLOCK_ASYNC_DIV8;
    g_adc1.Init.Resolution = ADC_RESOLUTION_16B;
    g_adc1.Init.ScanConvMode = ADC_SCAN_ENABLE;
    g_adc1.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
    g_adc1.Init.LowPowerAutoWait = DISABLE;
    g_adc1.Init.ContinuousConvMode = ENABLE;
    g_adc1.Init.NbrOfConversion = 3;
    g_adc1.Init.DiscontinuousConvMode = DISABLE;
    g_adc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
    g_adc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
    g_adc1.Init.ConversionDataManagement = ADC_CONVERSIONDATA_DMA_CIRCULAR;
    g_adc1.Init.Overrun = ADC_OVR_DATA_PRESERVED;
    g_adc1.Init.LeftBitShift = ADC_LEFTBITSHIFT_NONE;
    g_adc1.Init.OversamplingMode = DISABLE;
    g_adc1.Init.Oversampling.Ratio = 1;
    if (HAL_ADC_Init(&g_adc1) != HAL_OK)
    {
        error_handler();
    }

    /* ADC multi-mode: independent */
    multimode.Mode = ADC_MODE_INDEPENDENT;
    if (HAL_ADCEx_MultiModeConfigChannel(&g_adc1, &multimode) != HAL_OK)
    {
        error_handler();
    }

    /* Configure channel 10 (PC0) */
    sConfig.Channel = ADC_CHANNEL_10;
    sConfig.Rank = ADC_REGULAR_RANK_1;
    sConfig.SamplingTime = ADC_SAMPLETIME_810CYCLES_5;
    sConfig.SingleDiff = ADC_SINGLE_ENDED;
    sConfig.OffsetNumber = ADC_OFFSET_NONE;
    sConfig.Offset = 0;
    sConfig.OffsetSignedSaturation = DISABLE;
    if (HAL_ADC_ConfigChannel(&g_adc1, &sConfig) != HAL_OK)
    {
        error_handler();
    }

    /* Configure channel 11 (PC1) */
    sConfig.Channel = ADC_CHANNEL_11;
    sConfig.Rank = ADC_REGULAR_RANK_2;
    if (HAL_ADC_ConfigChannel(&g_adc1, &sConfig) != HAL_OK)
    {
        error_handler();
    }

    /* Configure channel 17 (PA1) */
    sConfig.Channel = ADC_CHANNEL_17;
    sConfig.Rank = ADC_REGULAR_RANK_3;
    if (HAL_ADC_ConfigChannel(&g_adc1, &sConfig) != HAL_OK)
    {
        error_handler();
    }
}

void HAL_ADC_MspInit(ADC_HandleTypeDef* adcHandle)
{

    GPIO_InitTypeDef GPIO_InitStruct = {0};
    if(adcHandle->Instance==ADC1)
    {
        /* ADC1 clock enable */
        __HAL_RCC_ADC12_CLK_ENABLE();

        __HAL_RCC_GPIOC_CLK_ENABLE();
        __HAL_RCC_GPIOA_CLK_ENABLE();

        /* ADC1 GPIO Configuration
           PC0 ------> ADC1_INP10
           PC1 ------> ADC1_INP11
           PA1 ------> ADC1_INP17 */
        GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1;
        GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

        GPIO_InitStruct.Pin = GPIO_PIN_1;
        GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

        /* ADC1 DMA configuration */
        g_hdma_adc1.Instance = DMA2_Stream6;
        g_hdma_adc1.Init.Request = DMA_REQUEST_ADC1;
        g_hdma_adc1.Init.Direction = DMA_PERIPH_TO_MEMORY;
        g_hdma_adc1.Init.PeriphInc = DMA_PINC_DISABLE;
        g_hdma_adc1.Init.MemInc = DMA_MINC_ENABLE;
        g_hdma_adc1.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
        g_hdma_adc1.Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;
        g_hdma_adc1.Init.Mode = DMA_NORMAL;
        g_hdma_adc1.Init.Priority = DMA_PRIORITY_LOW;
        g_hdma_adc1.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
        if (HAL_DMA_Init(&g_hdma_adc1) != HAL_OK)
        {
            error_handler();
        }

        __HAL_LINKDMA(adcHandle,DMA_Handle,g_hdma_adc1);

        /* ADC1 interrupt configuration */
        HAL_NVIC_SetPriority(ADC_IRQn, 5, 0);
        HAL_NVIC_EnableIRQ(ADC_IRQn);
    }
}

void HAL_ADC_MspDeInit(ADC_HandleTypeDef* adcHandle)
{

    if(adcHandle->Instance==ADC1)
    {
        __HAL_RCC_ADC12_CLK_DISABLE();

        /* ADC1 GPIO deinitialization
           PC0 ------> ADC1_INP10
           PC1 ------> ADC1_INP11
           PA1 ------> ADC1_INP17 */
        HAL_GPIO_DeInit(GPIOC, GPIO_PIN_0|GPIO_PIN_1);
        HAL_GPIO_DeInit(GPIOA, GPIO_PIN_1);

        /* ADC1 DMA deinitialization */
        HAL_DMA_DeInit(adcHandle->DMA_Handle);
    }
}
