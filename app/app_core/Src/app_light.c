/*
 * Copyright (c) 2026 beric-xiong
 * SPDX-License-Identifier: MIT
 *
 * LED strip task management module
 */

#include <stdio.h>
#include <string.h>
#include "main.h"
#include "light_dev.h"
#include "app_light.h"
#include "log_print.h"

#define TAG         "Light: "


#define APP_EN_COLOR_MAX        (15)

/* Color mode enumeration */
typedef enum
{
    APP_EN_COLOR_OFF    = 0,
    APP_EN_COLOR_RED     = 1,
    APP_EN_COLOR_RED_BREATHE,       /* Red breathe */
    APP_EN_COLOR_BLUE,              /* Blue */
    APP_EN_COLOR_BLUE_BREATHE,      /* Blue breathe */
    APP_EN_COLOR_GREEN = 5,         /* Green */
    APP_EN_LIGHT_OFFF,              /* Light off */
    APP_EN_COLOR_CYAN,              /* Cyan */
    APP_EN_COLOR_CYAN_BREATHE,      /* Cyan breathe */
    APP_EN_COLOR_YELLOW,            /* Yellow */
    APP_EN_COLOR_YELLOW_BREATHE = 10,   /* Yellow breathe */
    APP_EN_COLOR_WHITE,                 /* White */
    APP_EN_COLOR_WHITE_BREATHE,         /* White breathe */
    APP_EN_COLOR_PURPLE,                /* Purple */
    APP_EN_COLOR_PURPLE_BREATHE         /* Purple breathe */
} APP_COLOR_MODE_E;


QueueHandle_t g_light_queue;

/* Task creation definitions
 * Task stack size: 128*4 Bytes
 * Priority: 2
 */
#define         LIGHT_WORK_STK_SIZE                 128
#define         LIGHT_WORK_TASK_PRIO                2
TaskHandle_t    light_work_task_handler;
#define         BREATHE_LIGHT_WORK_STK_SIZE         128
#define         BREATHE_LIGHT_WORK_TASK_PRIO        2
TaskHandle_t    breathe_light_work_task_handler;

static Light_Device_T *light_dev = NULL;
static unsigned char s_color = APP_EN_COLOR_OFF;

static void queue_create_function(void)
{
    g_light_queue = xQueueCreate(5, sizeof(char));
}

/* Light work task — receives color commands via queue and applies them */
static void light_work_task_function(void *pvParameters)
{
    pvParameters = pvParameters;

    unsigned char msg[5] = {0};
    int ret = 0;
    unsigned char colorDefault = APP_EN_COLOR_OFF;

    /* Set default color on startup */
    ret = light_dev->set_light_color(light_dev, &colorDefault);
    if(ret == -1) {
        LOG_PRINT(LOG_OUT_ERROR, "%s set_light_color failed!\n", TAG);
    }

    while(1) {
        xQueueReceive(g_light_queue, msg, portMAX_DELAY);
        if(light_dev) {
            /* Breathe modes — resume breathe task */
            if((msg[0] == APP_EN_COLOR_RED_BREATHE) || (msg[0] == APP_EN_COLOR_BLUE_BREATHE) || (msg[0] == APP_EN_COLOR_CYAN_BREATHE)
            || (msg[0] == APP_EN_COLOR_YELLOW_BREATHE) || (msg[0] == APP_EN_COLOR_WHITE_BREATHE) || (msg[0] == APP_EN_COLOR_PURPLE_BREATHE))
            {
                s_color = msg[0];
                vTaskResume(breathe_light_work_task_handler);
            }
            else {
                /* Solid color — suspend breathe task */
                vTaskSuspend(breathe_light_work_task_handler);

                ret = light_dev->set_light_color(light_dev, msg);
                if(ret == -1) {
                    LOG_PRINT(LOG_OUT_ERROR, "%s set_light_color failed!\n", TAG);
                }
            }
        }
    }
}

/* Breathe light task — alternates between color and off */
static void breathe_light_work_task_function(void *pvParameters)
{
    pvParameters = pvParameters;

    int ret = 0;
    unsigned char uncolor = APP_EN_COLOR_OFF;

    while(1) {
        if(light_dev) {
            ret = light_dev->set_light_color(light_dev, &s_color);
            if(ret == -1) {
                LOG_PRINT(LOG_OUT_ERROR, "%s set_light_color failed!\n", TAG);
            }
            vTaskDelay(500);
            ret = light_dev->set_light_color(light_dev, &uncolor);
            if(ret == -1) {
                LOG_PRINT(LOG_OUT_ERROR, "%s set_light_color failed!\n", TAG);
            }
        }
        vTaskDelay(500);
    }
}

static void task_create_function(void)
{
    xTaskCreate((TaskFunction_t ) light_work_task_function,
                (const char*    ) "light_work_task_function",
                (uint16_t       ) LIGHT_WORK_STK_SIZE,
                (void*          ) NULL,
                (UBaseType_t    ) LIGHT_WORK_TASK_PRIO,
                (TaskHandle_t*  ) &light_work_task_handler);
    xTaskCreate((TaskFunction_t ) breathe_light_work_task_function,
                (const char*    ) "breathe_light_work_task_function",
                (uint16_t       ) BREATHE_LIGHT_WORK_STK_SIZE,
                (void*          ) NULL,
                (UBaseType_t    ) BREATHE_LIGHT_WORK_TASK_PRIO,
                (TaskHandle_t*  ) &breathe_light_work_task_handler);
    /* Suspend breathe task until a breathe color command is received */
    vTaskSuspend(breathe_light_work_task_handler);
}

void app_light_suspend(void)
{
    vTaskSuspend(light_work_task_handler);
    vTaskSuspend(breathe_light_work_task_handler);
}

void app_light_resume(void)
{
    vTaskResume(light_work_task_handler);
    vTaskResume(breathe_light_work_task_handler);
}

void app_light_init(void)
{
    light_dev = get_light_device("tm512ac");
    if(!light_dev) {
        LOG_PRINT(LOG_OUT_ERROR, "%s Can not find tm512ac!\n", TAG);
        return;
    }

    if(light_dev->init(light_dev)){
        LOG_PRINT(LOG_OUT_ERROR, "%s TM512ac init error!\n", TAG);
        return;
    }

    queue_create_function();
    task_create_function();
}
