/*
 * Copyright (c) 2026 beric-xiong
 * SPDX-License-Identifier: MIT
 *
 * FreeRTOSConfig
 */

#include "main.h" 
#include "FreeRTOS.h"				  
#include "task.h" 

/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
extern void xPortSysTickHandler(void);

void SysTick_Handler(void)
{
    uint32_t ulReturn;
    /* Enter critical section, nestable */
    ulReturn = taskENTER_CRITICAL_FROM_ISR();
  
    HAL_IncTick();
#if ( ( INCLUDE_xTaskGetSchedulerState == 1 ) || ( configUSE_TIMERS == 1 ) )
    if (xTaskGetSchedulerState() != taskSCHEDULER_NOT_STARTED)
    {
#endif  /* INCLUDE_xTaskGetSchedulerState */  
        xPortSysTickHandler();
#if ( ( INCLUDE_xTaskGetSchedulerState == 1 ) || ( configUSE_TIMERS == 1 ) )
    }
#endif  /* INCLUDE_xTaskGetSchedulerState */
  
    /* Exit critical section */
    taskEXIT_CRITICAL_FROM_ISR( ulReturn );
}

