/*
 * Copyright (c) 2026 beric-xiong
 * SPDX-License-Identifier: MIT
 *
 * TIM4 input capture initialization (2-channel)
 * CH1=PD12, CH2=PD13 (AF2)
 */

#include "bsp_cap.h"

/* APB1 timer clock = 200 MHz, prescale to 1 MHz (1 µs resolution) */
#define CAP_TIMER_CLK_HZ     200000000UL
#define CAP_TIMER_PRESCALER  199U     /* 200MHz / (199+1) = 1MHz */
#define CAP_TIMER_PERIOD     0xFFFFU  /* max 16-bit range */

TIM_HandleTypeDef g_tim4;

void bsp_cap4_init(void)
{
    TIM_IC_InitTypeDef sConfigIC = {0};
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    /* Enable GPIOD clock (PD12, PD13) */
    __HAL_RCC_GPIOD_CLK_ENABLE();
    /* Enable TIM4 clock */
    __HAL_RCC_TIM4_CLK_ENABLE();

    /* GPIOD pins 12, 13 → AF2 (TIM4_CH1-2) */
    GPIO_InitStruct.Pin = GPIO_PIN_12 | GPIO_PIN_13;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF2_TIM4;
    HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

    /* Configure TIM4 base: 1 MHz clock, max range */
    g_tim4.Instance = TIM4;
    g_tim4.Init.Prescaler = CAP_TIMER_PRESCALER;
    g_tim4.Init.CounterMode = TIM_COUNTERMODE_UP;
    g_tim4.Init.Period = CAP_TIMER_PERIOD;
    g_tim4.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    g_tim4.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
    if (HAL_TIM_IC_Init(&g_tim4) != HAL_OK)
    {
        error_handler();
    }

    /* Configure CH1 and CH2: direct input, falling edge, no filter */
    sConfigIC.ICPolarity = TIM_INPUTCHANNELPOLARITY_FALLING;
    sConfigIC.ICSelection = TIM_ICSELECTION_DIRECTTI;
    sConfigIC.ICPrescaler = TIM_ICPSC_DIV1;
    sConfigIC.ICFilter = 0;

    HAL_TIM_IC_ConfigChannel(&g_tim4, &sConfigIC, TIM_CHANNEL_1);
    HAL_TIM_IC_ConfigChannel(&g_tim4, &sConfigIC, TIM_CHANNEL_2);

    /* Enable capture interrupt */
    HAL_NVIC_SetPriority(TIM4_IRQn, 5, 0);
    HAL_NVIC_EnableIRQ(TIM4_IRQn);
}
