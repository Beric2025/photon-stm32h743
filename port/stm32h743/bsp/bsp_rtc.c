/*
 * Copyright (c) 2026 beric-xiong
 * SPDX-License-Identifier: MIT
 *
 * STM32 RTC initialization using LSE (32.768 kHz)
 */

#include "bsp_rtc.h"

RTC_HandleTypeDef g_rtc;

void bsp_rtc_init(void)
{
    /* Enable LSE and configure RTC */
    HAL_PWR_EnableBkUpAccess();

    /* LSE configuration */
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_LSE;
    RCC_OscInitStruct.LSEState = RCC_LSE_ON;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
    {
        error_handler();
    }

    /* RTC clock source: LSE */
    RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};
    PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_RTC;
    PeriphClkInitStruct.RTCClockSelection = RCC_RTCCLKSOURCE_LSE;
    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
    {
        error_handler();
    }

    /* Enable RTC clock */
    __HAL_RCC_RTC_ENABLE();

    g_rtc.Instance = RTC;
    g_rtc.Init.HourFormat = RTC_HOURFORMAT_24;
    g_rtc.Init.AsynchPrediv = 127;   /* 32768 / (127+1) = 256 Hz */
    g_rtc.Init.SynchPrediv = 255;    /* 256 / (255+1) = 1 Hz */
    g_rtc.Init.OutPut = RTC_OUTPUT_DISABLE;
    g_rtc.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
    g_rtc.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;
    if (HAL_RTC_Init(&g_rtc) != HAL_OK)
    {
        error_handler();
    }
}
