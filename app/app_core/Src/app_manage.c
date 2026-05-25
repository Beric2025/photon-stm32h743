/*
 * Copyright (c) 2026 beric-xiong
 * SPDX-License-Identifier: MIT
 *
 * Task management module
 */

#include <stdio.h>
#include <string.h>
#include "main.h"
#include "app_manage.h"
#include "app_version.h"
#include "lwip_port.h"
#include "app_led.h"
#include "app_network.h"
#include "app_light.h"
#include "app_motor.h"
#include "app_battery.h"
#include "app_ultrasonic.h"
#include "app_adc_battery.h"

#define TAG         "app manage: "


/* OS running status: 0-not running, 1-running */
uint8_t is_os_running;     

/* static void start_task_function( void * parameters ) __attribute__( ( noreturn ) );    //example */


static void device_init(void)
{
    char version[100] = {0};

    log_print_init();

    build_version();
    get_version(version,sizeof(version));
    LOG_PRINT(LOG_OUT_INFO, "%sApp Ver:%s\n",TAG, version);
}


static void task_init(void)
{
    device_init();
    app_network_init();
    taskENTER_CRITICAL();
    app_led_init();
    app_light_init();
    app_motor_init();
    app_battery_init();
    app_adc_battery_init();
    app_ultrasonic_init();
    taskEXIT_CRITICAL();
}


static void start_task_function(void *parameters)
{
	parameters = parameters;

    task_init();

	vTaskDelete (NULL);
}


void application_init(void)
{
    xTaskCreate((TaskFunction_t ) start_task_function, NULL, 512, NULL, 0, NULL);
    is_os_running = 1;
    vTaskStartScheduler();
}


