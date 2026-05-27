/*
 * Copyright (c) 2026 beric-xiong
 * SPDX-License-Identifier: MIT
 *
 * TIM3 PWM initialization (4-channel)
 * CH1=PB4, CH2=PB5, CH3=PB0, CH4=PB1 (AF2)
 */

#include "bsp_pwm.h"

/* APB1 timer clock = 200 MHz (system clock, APB1 prescaler = 2, timers get x2) */
#define TIM3_BASE_CLK_HZ  200000000UL
#define PWM_ARR           999U     /* 1000-step resolution for duty cycle */

TIM_HandleTypeDef g_tim3;

void bsp_pwm3_init(unsigned int freq_hz)
{
    TIM_OC_InitTypeDef sConfigOC = {0};
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    /* Enable GPIOB clock (PB4, PB5, PB0, PB1) */
    __HAL_RCC_GPIOB_CLK_ENABLE();
    /* Enable TIM3 clock */
    __HAL_RCC_TIM3_CLK_ENABLE();

    /* GPIOB pins 4, 5, 0, 1 → AF2 (TIM3_CH1-4) */
    GPIO_InitStruct.Pin = GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_0 | GPIO_PIN_1;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF2_TIM3;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    /* Calculate prescaler to hit target frequency with PWM_ARR steps */
    unsigned int prescaler = (TIM3_BASE_CLK_HZ / (freq_hz * (PWM_ARR + 1))) - 1;

    g_tim3.Instance = TIM3;
    g_tim3.Init.Prescaler = prescaler;
    g_tim3.Init.CounterMode = TIM_COUNTERMODE_UP;
    g_tim3.Init.Period = PWM_ARR;
    g_tim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    g_tim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
    if (HAL_TIM_PWM_Init(&g_tim3) != HAL_OK)
    {
        error_handler();
    }

    /* Configure each channel: PWM mode 1, 0% duty initially */
    sConfigOC.OCMode = TIM_OCMODE_PWM1;
    sConfigOC.Pulse = 0;
    sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
    sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;

    sConfigOC.OCIdleState = TIM_OCIDLESTATE_RESET;
    HAL_TIM_PWM_ConfigChannel(&g_tim3, &sConfigOC, TIM_CHANNEL_1);
    HAL_TIM_PWM_ConfigChannel(&g_tim3, &sConfigOC, TIM_CHANNEL_2);
    HAL_TIM_PWM_ConfigChannel(&g_tim3, &sConfigOC, TIM_CHANNEL_3);
    HAL_TIM_PWM_ConfigChannel(&g_tim3, &sConfigOC, TIM_CHANNEL_4);
}


TIM_HandleTypeDef g_tim5;

void bsp_tim5_init(unsigned int freq_hz)
{
    (void)freq_hz;

    /* Enable TIM5 clock */
    __HAL_RCC_TIM5_CLK_ENABLE();

    /* 1 MHz free-running counter (1 tick = 1 µs), 32-bit max range */
    g_tim5.Instance = TIM5;
    g_tim5.Init.Prescaler = 199;                     /* 200 MHz / 200 = 1 MHz */
    g_tim5.Init.CounterMode = TIM_COUNTERMODE_UP;
    g_tim5.Init.Period = 0xFFFFFFFF;                 /* 32-bit, wraps every ~71 min */
    g_tim5.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    g_tim5.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
    if (HAL_TIM_Base_Init(&g_tim5) != HAL_OK)
    {
        error_handler();
    }
}
