/*
 * Copyright (c) 2026 beric-xiong
 * SPDX-License-Identifier: MIT
 *
 * SPI initialization file
 */

#include <string.h>
#include <stdio.h>
#include "bsp_spi.h"

SPI_HandleTypeDef g_spi1;


void bsp_spi1_init(void)
{
  g_spi1.Instance = SPI1;
  g_spi1.Init.Mode = SPI_MODE_MASTER;
  g_spi1.Init.Direction = SPI_DIRECTION_2LINES;
  g_spi1.Init.DataSize = SPI_DATASIZE_8BIT;
  g_spi1.Init.CLKPolarity = SPI_POLARITY_LOW;
  g_spi1.Init.CLKPhase = SPI_PHASE_1EDGE;
  g_spi1.Init.NSS = SPI_NSS_SOFT;
  g_spi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_8;
  g_spi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  g_spi1.Init.TIMode = SPI_TIMODE_DISABLE;
  g_spi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  g_spi1.Init.CRCPolynomial = 0x0;
  g_spi1.Init.NSSPMode = SPI_NSS_PULSE_ENABLE;
  g_spi1.Init.NSSPolarity = SPI_NSS_POLARITY_LOW;
  g_spi1.Init.FifoThreshold = SPI_FIFO_THRESHOLD_01DATA;
  g_spi1.Init.TxCRCInitializationPattern = SPI_CRC_INITIALIZATION_ALL_ZERO_PATTERN;
  g_spi1.Init.RxCRCInitializationPattern = SPI_CRC_INITIALIZATION_ALL_ZERO_PATTERN;
  g_spi1.Init.MasterSSIdleness = SPI_MASTER_SS_IDLENESS_00CYCLE;
  g_spi1.Init.MasterInterDataIdleness = SPI_MASTER_INTERDATA_IDLENESS_00CYCLE;
  g_spi1.Init.MasterReceiverAutoSusp = SPI_MASTER_RX_AUTOSUSP_DISABLE;
  g_spi1.Init.MasterKeepIOState = SPI_MASTER_KEEP_IO_STATE_DISABLE;
  g_spi1.Init.IOSwap = SPI_IO_SWAP_DISABLE;
  if (HAL_SPI_Init(&g_spi1) != HAL_OK)
  {
    error_handler();
  }

}

void HAL_SPI_MspInit(SPI_HandleTypeDef* spiHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};
  if(spiHandle->Instance==SPI1)
  {

  /** Initializes the peripherals clock
  */
    PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_SPI1;
    PeriphClkInitStruct.Spi123ClockSelection = RCC_SPI123CLKSOURCE_PLL;
    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
    {
      error_handler();
    }

    /* SPI1 clock enable */
    __HAL_RCC_SPI1_CLK_ENABLE();

    __HAL_RCC_GPIOA_CLK_ENABLE();
    /**SPI1 GPIO Configuration
    PA5     ------> SPI1_SCK
    PA6     ------> SPI1_MISO
    PA7     ------> SPI1_MOSI
    */
    GPIO_InitStruct.Pin = GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF5_SPI1;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
  }
}

void HAL_SPI_MspDeInit(SPI_HandleTypeDef* spiHandle)
{

  if(spiHandle->Instance==SPI1)
  {
    /* Peripheral clock disable */
    __HAL_RCC_SPI1_CLK_DISABLE();

    /**SPI1 GPIO Configuration
    PA5     ------> SPI1_SCK
    PA6     ------> SPI1_MISO
    PA7     ------> SPI1_MOSI
    */
    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7);

  }
}

