/*
 * Copyright (c) 2026 beric-xiong
 * SPDX-License-Identifier: MIT
 *
 * Custom delay functions
 */

	
#include "stm32h7xx_hal.h"
#include "delay_dev.h"

/* #define USE_SYSTICK */

#ifndef USE_SYSTICK
extern TIM_HandleTypeDef htim6;
#endif

void delay_us(unsigned int us)
{
#ifdef USE_SYSTICK
	unsigned int told = SysTick->VAL;
	unsigned int tnow = 0;
	unsigned int cnt = 0, tick = us*(SysTick->LOAD + 1)/1000;
	
	while(1) {
		tnow = SysTick->VAL;
		if(tnow <= told)
			cnt += told - tnow;
		else
			cnt += told + SysTick->LOAD - tnow + 1;
		told = tnow;
		
		if(cnt >= tick)
			break;
	}
#else
	unsigned int told = __HAL_TIM_GET_COUNTER(&htim6);
	unsigned int tnow = 0;
	unsigned int cnt = 0, tick = us*(__HAL_TIM_GET_AUTORELOAD(&htim6) + 1)/1000;
	
	while(1) {
		tnow = __HAL_TIM_GET_COUNTER(&htim6);
		if(tnow >= told)
			cnt += tnow - told;
		else
			cnt += tnow + 1 + __HAL_TIM_GET_AUTORELOAD(&htim6) - told;
		told = tnow;
		
		if(cnt >= tick)
			break;
	}
#endif
}


void delay_ms(unsigned int ms)
{
	while(ms--){
		delay_us(1000);
	}
}

