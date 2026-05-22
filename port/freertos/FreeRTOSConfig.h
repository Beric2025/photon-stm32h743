/*
 * Copyright (c) 2026 beric-xiong
 * SPDX-License-Identifier: MIT
 */

#ifndef FREERTOS_CONFIG_H
#define FREERTOS_CONFIG_H

#if defined(__ICCARM__ )|| defined(__CC_ARM) || defined(__GNUC__)
	#include <stdint.h>
	extern uint32_t SystemCoreClock;
#endif

/* Base config options */
#define configUSE_PREEMPTION                            1                           /* 1: preemptive, 0: cooperative. No default, must define */
#define configUSE_PORT_OPTIMISED_TASK_SELECTION         1                           /* 1: use hardware for next task, 0: use software. Default: 0 */
#define configUSE_TICKLESS_IDLE                         0                           /* 1: enable tickless idle. Default: 0 */
#define configCPU_CLOCK_HZ                              SystemCoreClock             /* CPU clock frequency in Hz. No default, must define */
// #define configSYSTICK_CLOCK_HZ                          (configCPU_CLOCK_HZ / 8) /* SysTick clock frequency in Hz. Only define when different from CPU clock. Default: undefined */
#define configTICK_RATE_HZ				                ((TickType_t) 1000)         /* SysTick frequency in Hz. No default, must define */
#define configMAX_PRIORITIES                            32                          /* Max priority value = configMAX_PRIORITIES - 1. No default, must define */
#define configMINIMAL_STACK_SIZE                        ((unsigned short) 100)      /* Idle task stack size in bytes. No default, must define */
#define configMAX_TASK_NAME_LEN                         16                          /* Max task name length. Default: 16 */
#define configTICK_TYPE_WIDTH_IN_BITS                   TICK_TYPE_WIDTH_32_BITS    /* Tick counter width: 16/32/64 bits. No default, must define */
#define configIDLE_SHOULD_YIELD                         1                           /* 1: same-priority tasks can preempt idle task. Default: 1 */
#define configUSE_TASK_NOTIFICATIONS                    1                           /* 1: enable task notifications (semaphores, event groups, mailboxes). Default: 1 */
#define configTASK_NOTIFICATION_ARRAY_ENTRIES           1                           /* Task notification array size. Default: 1 */
#define configUSE_MUTEXES                               1                           /* 1: enable mutexes. Default: 0 */
#define configUSE_RECURSIVE_MUTEXES                     1                           /* 1: enable recursive mutexes. Default: 0 */
#define configUSE_COUNTING_SEMAPHORES                   1                           /* 1: enable counting semaphores. Default: 0 */
#define configUSE_ALTERNATIVE_API                       0                           /* Deprecated! */
#define configQUEUE_REGISTRY_SIZE                       8                           /* Number of registrable semaphores/queues. Default: 0 */
#define configUSE_QUEUE_SETS                            1                           /* 1: enable queue sets. Default: 0 */
#define configUSE_TIME_SLICING                          1                           /* 1: enable time slice scheduling. Default: 1 */
#define configUSE_NEWLIB_REENTRANT                      0                           /* 1: allocate Newlib reent struct on task create. Default: 0 */
#define configENABLE_BACKWARD_COMPATIBILITY             1                           /* 1: enable backward compatibility. Default: 0 */
#define configNUM_THREAD_LOCAL_STORAGE_POINTERS         0                           /* Number of thread-local storage pointers. Default: 0 */
#define configSTACK_DEPTH_TYPE                          uint16_t                    /* Task stack depth data type. Default: uint16_t */
#define configMESSAGE_BUFFER_LENGTH_TYPE                size_t                      /* Message length type. Default: size_t */
#define configUSE_MINI_LIST_ITEM                        1                           /* 1: use mini list item to save RAM. Default: 1 */

/* Memory allocation */
#define configSUPPORT_STATIC_ALLOCATION                 0                       /* 1: support static allocation. Default: 0 */
#define configSUPPORT_DYNAMIC_ALLOCATION                1                       /* 1: support dynamic allocation. Default: 1 */
#define configTOTAL_HEAP_SIZE                           ((size_t)(10 * 1024))   /* Total FreeRTOS heap size in bytes. No default, must define */
#define configAPPLICATION_ALLOCATED_HEAP                0                       /* 1: user manually allocates heap (ucHeap). Default: 0 */
#define configSTACK_ALLOCATION_FROM_SEPARATE_HEAP       0                       /* 1: user implements stack alloc/free functions. Default: 0 */
#define configHEAP_CLEAR_MEMORY_ON_FREE                 0                       /* 1: clear memory on free. Default: 0 */
#define configENABLE_HEAP_PROTECTOR                     0                       /* 1: enable heap block boundary check (heap_4/5 only). Default: 0 */

/* Event group */
#define configUSE_EVENT_GROUPS                          1                           /* 1: enable event groups. Default: 1 */

/* Stream buffer */
#define configUSE_STREAM_BUFFERS                        1                           /* 1: enable stream buffers. Default: 1 */
#define configUSE_SB_COMPLETED_CALLBACK                 0                           /* 1: enable send/recv complete callbacks. Default: 0 */

/* Hook functions */
#define configUSE_IDLE_HOOK                             0                       /* 1: enable idle task hook. No default, must define */
#define configUSE_TICK_HOOK                             0                       /* 1: enable SysTick hook. No default, must define */
#define configCHECK_FOR_STACK_OVERFLOW                  0                       /* 1: method 1, 2: method 2. Default: 0 */
#define configUSE_MALLOC_FAILED_HOOK                    0                       /* 1: enable malloc failed hook. Default: 0 */
#define configUSE_DAEMON_TASK_STARTUP_HOOK              0                       /* 1: enable timer task startup hook. Default: 0 */

/* Runtime and task stats */
#define configGENERATE_RUN_TIME_STATS                   0                       /* 1: enable runtime stats. Default: 0 */
#define configUSE_TRACE_FACILITY                        0                       /* 1: enable visual trace debug. Default: 0 */
#define configUSE_STATS_FORMATTING_FUNCTIONS            0                       /* 1: compile vTaskList/vTaskGetRunTimeStats when TRACE_FACILITY=1. Default: 0 */
#define configSTATS_BUFFER_MAX_LENGTH                   0xFFFF                  /* Buffer size for vTaskList/vTaskGetRunTimeStats APIs. Default: 0xFFFF */

/* Co-routines */
#define configUSE_CO_ROUTINES                           0                       /* 1: enable co-routine functions. Default: 0 */
#define configMAX_CO_ROUTINE_PRIORITIES                 2                       /* Max co-routine priority. Must define when USE_CO_ROUTINES=1 */

/* Software timers */
#define configUSE_TIMERS                                1                               /* 1: enable software timers. Default: 0 */
#define configTIMER_TASK_PRIORITY                       ( configMAX_PRIORITIES - 1 )    /* Timer task priority. Must define when USE_TIMERS=1 */
#define configTIMER_QUEUE_LENGTH                        5                               /* Timer command queue length. Must define when USE_TIMERS=1 */
#define configTIMER_TASK_STACK_DEPTH                    ( configMINIMAL_STACK_SIZE * 2) /* Timer task stack size. Must define when USE_TIMERS=1 */

/* Optional API includes (1 = enabled) */
#define INCLUDE_vTaskPrioritySet                        1                       /* Set task priority */
#define INCLUDE_uxTaskPriorityGet                       1                       /* Get task priority */
#define INCLUDE_vTaskDelete                             1                       /* Delete task */
#define INCLUDE_vTaskSuspend                            1                       /* Suspend task */
#define INCLUDE_xResumeFromISR                          1                       /* Resume task from ISR */
#define INCLUDE_vTaskDelayUntil                         1                       /* Absolute task delay */
#define INCLUDE_vTaskDelay                              1                       /* Relative task delay */
#define INCLUDE_xTaskGetSchedulerState                  1                       /* Get scheduler state */
#define INCLUDE_xTaskGetCurrentTaskHandle               1                       /* Get current task handle */
#define INCLUDE_uxTaskGetStackHighWaterMark             1                       /* Get stack high water mark */
#define INCLUDE_xTaskGetIdleTaskHandle                  1                       /* Get idle task handle */
#define INCLUDE_eTaskGetState                           1                       /* Get task state */
#define INCLUDE_xEventGroupSetBitFromISR                1                       /* Set event group bit from ISR */
#define INCLUDE_xTimerPendFunctionCall                  1                       /* Pend function call to timer service */
#define INCLUDE_xTaskAbortDelay                         1                       /* Abort task delay */
#define INCLUDE_xTaskGetHandle                          1                       /* Get task handle by name */
#define INCLUDE_xTaskResumeFromISR                      1                       /* Resume suspended task from ISR */

/* Interrupt nesting behavior */
#ifdef __NVIC_PRIO_BITS
    #define configPRIO_BITS __NVIC_PRIO_BITS
#else
    #define configPRIO_BITS 4
#endif

#define configLIBRARY_LOWEST_INTERRUPT_PRIORITY         15                      /* Lowest interrupt priority */
#define configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY    5                       /* Max FreeRTOS-managed interrupt priority */
/* Convert library priority to hardware priority. STM32 NVIC uses high configPRIO_BITS bits of 8-bit register. Shift left (8 - configPRIO_BITS) to get correct hardware value */
#define configKERNEL_INTERRUPT_PRIORITY                 ( configLIBRARY_LOWEST_INTERRUPT_PRIORITY << (8 - configPRIO_BITS) )
/* FreeRTOS APIs can be safely called at this priority and below. Higher priorities are never masked */
#define configMAX_SYSCALL_INTERRUPT_PRIORITY            ( configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY << (8 - configPRIO_BITS) )
/* API call interrupt priority -- same as syscall for compatibility */
#define configMAX_API_CALL_INTERRUPT_PRIORITY           configMAX_SYSCALL_INTERRUPT_PRIORITY

/* Map FreeRTOS PendSV/SVC handlers to STM32 standard names */
#define xPortPendSVHandler                              PendSV_Handler
#define vPortSVCHandler                                 SVC_Handler

/* POSIX errno */
#define configUSE_POSIX_ERRNO                           0                           /* 1: enable per-task FreeRTOS_errno. Default: 0 */

/* Assert config -- empty by default. Uncomment below for file/line error output */
// #include <stdint.h>
// #define vAssertCalled(char, int) printf("Error: %s, %d\n", char, int)
// #define configASSERT( x ) if( ( x ) == 0 ) vAssertCalled( __FILE__, __LINE__ )
#define configASSERT( x ) if( ( x ) == 0 ) {}

/* FreeRTOS MPU definitions -- only needed when MPU is used */
//#define configINCLUDE_APPLICATION_DEFINED_PRIVILEGED_FUNCTIONS 0                  /* 1: allow app-defined privileged functions. Default: 0 */
//#define configTOTAL_MPU_REGIONS                                8                  /* Total MPU regions, must match hardware. Default: 8 */
//#define configTEX_S_C_B_FLASH                                  0x07UL             /* Flash MPU attributes (TEX, S, C, B). Default: 0x07UL */
//#define configTEX_S_C_B_SRAM                                   0x07UL             /* SRAM MPU attributes (TEX, S, C, B). Default: 0x07UL */
//#define configENFORCE_SYSTEM_CALLS_FROM_KERNEL_ONLY            1                  /* 1: syscalls from kernel mode only. Default: 1 */
//#define configALLOW_UNPRIVILEGED_CRITICAL_SECTIONS             1                  /* 1: allow unprivileged critical sections. Default: 1 */

/* ARMv8-M secure side definitions -- for TrustZone */
// #define secureconfigMAX_SECURE_CONTEXTS                       5                  /* Max secure contexts for TrustZone context switch. Default: 5 */


#endif /* FREERTOS_CONFIG_H */
