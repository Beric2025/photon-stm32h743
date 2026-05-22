/*
 * Copyright (c) 2026 beric-xiong
 * SPDX-License-Identifier: MIT
 *
 * I2C initialization file
 */

#include <string.h>
#include <stdio.h>
#include "bsp_i2c.h"


I2C_HandleTypeDef g_i2c2;

/**
 * bsp_i2c2_init - I2C2 initialization, baud rate 100kbps
 */
void bsp_i2c2_init(unsigned int baudrate)
{
  baudrate = baudrate;

  g_i2c2.Instance = I2C2;
  g_i2c2.Init.Timing = 0x10C0ECFF;
  g_i2c2.Init.OwnAddress1 = 0;
  g_i2c2.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  g_i2c2.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  g_i2c2.Init.OwnAddress2 = 0;
  g_i2c2.Init.OwnAddress2Masks = I2C_OA2_NOMASK;
  g_i2c2.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  g_i2c2.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&g_i2c2) != HAL_OK)
  {
    error_handler();
  }

  /** Configure Analogue filter
  */
  if (HAL_I2CEx_ConfigAnalogFilter(&g_i2c2, I2C_ANALOGFILTER_ENABLE) != HAL_OK)
  {
    error_handler();
  }

  /** Configure Digital filter
  */
  if (HAL_I2CEx_ConfigDigitalFilter(&g_i2c2, 0) != HAL_OK)
  {
    error_handler();
  }


}

void HAL_I2C_MspInit(I2C_HandleTypeDef* i2cHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};
  if(i2cHandle->Instance==I2C2)
  {

  /** Initializes the peripherals clock
  */
    PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_I2C2;
    PeriphClkInitStruct.I2c123ClockSelection = RCC_I2C123CLKSOURCE_D2PCLK1;
    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
    {
      error_handler();
    }

    __HAL_RCC_GPIOH_CLK_ENABLE();
    /**I2C2 GPIO Configuration
    PH4     ------> I2C2_SCL
    PH5     ------> I2C2_SDA
    */
    GPIO_InitStruct.Pin = GPIO_PIN_4|GPIO_PIN_5;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF4_I2C2;
    HAL_GPIO_Init(GPIOH, &GPIO_InitStruct);

    __HAL_RCC_I2C2_CLK_ENABLE();

  }
}

void HAL_I2C_MspDeInit(I2C_HandleTypeDef* i2cHandle)
{

  if(i2cHandle->Instance==I2C2)
  {
    __HAL_RCC_I2C2_CLK_DISABLE();

    /**I2C2 GPIO Configuration
    PH4     ------> I2C2_SCL
    PH5     ------> I2C2_SDA
    */
    HAL_GPIO_DeInit(GPIOH, GPIO_PIN_4);

    HAL_GPIO_DeInit(GPIOH, GPIO_PIN_5);
  }
}


