/*
 * Copyright (c) 2026 beric-xiong
 * SPDX-License-Identifier: MIT
 *
 * UART initialization
 */

#include <string.h>
#include <stdio.h>
#include "bsp_uart.h"

/* UART handle definition */
UART_HandleTypeDef g_uart1;
UART_HandleTypeDef g_uart4;
UART_HandleTypeDef g_uart7;

static DMA_HandleTypeDef s_hdma_uart7_rx;
static DMA_HandleTypeDef s_hdma_uart7_tx;

void bsp_uart1_init(unsigned int baudrate)
{
    g_uart1.Instance = USART1;
    g_uart1.Init.BaudRate = baudrate;
    g_uart1.Init.WordLength = UART_WORDLENGTH_8B;
    g_uart1.Init.StopBits = UART_STOPBITS_1;
    g_uart1.Init.Parity = UART_PARITY_NONE;
    g_uart1.Init.Mode = UART_MODE_TX_RX;
    g_uart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    g_uart1.Init.OverSampling = UART_OVERSAMPLING_16;
    g_uart1.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
    g_uart1.Init.ClockPrescaler = UART_PRESCALER_DIV1;
    g_uart1.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
    if (HAL_UART_Init(&g_uart1) != HAL_OK)
    {
      error_handler();
    }
    if (HAL_UARTEx_SetTxFifoThreshold(&g_uart1, UART_TXFIFO_THRESHOLD_1_8) != HAL_OK)
    {
      error_handler();
    }
    if (HAL_UARTEx_SetRxFifoThreshold(&g_uart1, UART_RXFIFO_THRESHOLD_1_8) != HAL_OK)
    {
      error_handler();
    }
    if (HAL_UARTEx_DisableFifoMode(&g_uart1) != HAL_OK)
    {
      error_handler();
    }
}
void bsp_uart4_init(unsigned int baudRate)
{
    g_uart4.Instance = UART4;
    g_uart4.Init.BaudRate = baudRate;
    g_uart4.Init.WordLength = UART_WORDLENGTH_8B;
    // g_uart4.Init.StopBits = UART_STOPBITS_1;
     g_uart4.Init.StopBits = UART_STOPBITS_2;
    g_uart4.Init.Parity = UART_PARITY_NONE;
    g_uart4.Init.Mode = UART_MODE_TX_RX;
    g_uart4.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    g_uart4.Init.OverSampling = UART_OVERSAMPLING_16;
    g_uart4.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
    g_uart4.Init.ClockPrescaler = UART_PRESCALER_DIV1;
    g_uart4.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
    if (HAL_UART_Init(&g_uart4) != HAL_OK)
    {
      error_handler();
    }
    if (HAL_UARTEx_SetTxFifoThreshold(&g_uart4, UART_TXFIFO_THRESHOLD_1_8) != HAL_OK)
    {
      error_handler();
    }
    if (HAL_UARTEx_SetRxFifoThreshold(&g_uart4, UART_RXFIFO_THRESHOLD_1_8) != HAL_OK)
    {
      error_handler();
    }
    if (HAL_UARTEx_DisableFifoMode(&g_uart4) != HAL_OK)
    {
      error_handler();
    }
}
void bsp_uart7_init(unsigned int baudRate)
{
    /* DMA controller clock enable */
    __HAL_RCC_DMA2_CLK_ENABLE();

    /* DMA interrupt init */
    /* DMA1_Stream0_IRQn interrupt configuration */
    HAL_NVIC_SetPriority(DMA2_Stream0_IRQn, 5, 0);
    HAL_NVIC_EnableIRQ(DMA2_Stream0_IRQn);
    /* DMA1_Stream1_IRQn interrupt configuration */
    HAL_NVIC_SetPriority(DMA2_Stream1_IRQn, 5, 0);
    HAL_NVIC_EnableIRQ(DMA2_Stream1_IRQn);
    
    g_uart7.Instance = UART7;
    g_uart7.Init.BaudRate = baudRate;
    g_uart7.Init.WordLength = UART_WORDLENGTH_8B;
    g_uart7.Init.StopBits = UART_STOPBITS_1;
    g_uart7.Init.Parity = UART_PARITY_NONE;
    g_uart7.Init.Mode = UART_MODE_TX_RX;
    g_uart7.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    g_uart7.Init.OverSampling = UART_OVERSAMPLING_16;
    g_uart7.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
    g_uart7.Init.ClockPrescaler = UART_PRESCALER_DIV1;
    g_uart7.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
    if (HAL_UART_Init(&g_uart7) != HAL_OK)
    {
      error_handler();
    }
    if (HAL_UARTEx_SetTxFifoThreshold(&g_uart7, UART_TXFIFO_THRESHOLD_1_8) != HAL_OK)
    {
      error_handler();
    }
    if (HAL_UARTEx_SetRxFifoThreshold(&g_uart7, UART_RXFIFO_THRESHOLD_1_8) != HAL_OK)
    {
      error_handler();
    }
    if (HAL_UARTEx_DisableFifoMode(&g_uart7) != HAL_OK)
    {
      error_handler();
    }
}

void HAL_UART_MspInit(UART_HandleTypeDef* uartHandle)
{

    GPIO_InitTypeDef GPIO_InitStruct = {0};
    RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};
    if(uartHandle->Instance==USART1)
    {

    /** Initializes the peripherals clock
    */
      PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_USART1;
      PeriphClkInitStruct.Usart16ClockSelection = RCC_USART16CLKSOURCE_D2PCLK2;
      if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
      {
        error_handler();
      }

      __HAL_RCC_USART1_CLK_ENABLE();

      __HAL_RCC_GPIOA_CLK_ENABLE();
      /**USART1 GPIO Configuration
      PA9     ------> USART1_TX
      PA10     ------> USART1_RX
      */
      GPIO_InitStruct.Pin = UART_TX_DEBUG_Pin|UART_RX_DEBUG_Pin;
      GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
      GPIO_InitStruct.Pull = GPIO_NOPULL;
      GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
      GPIO_InitStruct.Alternate = GPIO_AF7_USART1;
      HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
    }
    else if(uartHandle->Instance==UART4)
    {
    /* USER CODE BEGIN UART4_MspInit 0 */

    /* USER CODE END UART4_MspInit 0 */

    /** Initializes the peripherals clock
    */
      PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_UART4;
      PeriphClkInitStruct.Usart234578ClockSelection = RCC_USART234578CLKSOURCE_D2PCLK1;
      if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
      {
        error_handler();
      }

      /* UART4 clock enable */
      __HAL_RCC_UART4_CLK_ENABLE();

      __HAL_RCC_GPIOI_CLK_ENABLE();
      __HAL_RCC_GPIOA_CLK_ENABLE();
      /**UART4 GPIO Configuration
      PI9     ------> UART4_RX
      PA0     ------> UART4_TX
      */
      GPIO_InitStruct.Pin = GPIO_PIN_9;
      GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
      GPIO_InitStruct.Pull = GPIO_NOPULL;
      GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
      GPIO_InitStruct.Alternate = GPIO_AF8_UART4;
      HAL_GPIO_Init(GPIOI, &GPIO_InitStruct);

      GPIO_InitStruct.Pin = GPIO_PIN_0;
      GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
      GPIO_InitStruct.Pull = GPIO_NOPULL;
      GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
      GPIO_InitStruct.Alternate = GPIO_AF8_UART4;
      HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
    }
    else if(uartHandle->Instance==UART7)
    {
    /** Initializes the peripherals clock
    */
      PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_UART7;
      PeriphClkInitStruct.Usart234578ClockSelection = RCC_USART234578CLKSOURCE_D2PCLK1;
      if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
      {
        error_handler();
      }

      /* UART7 clock enable */
      __HAL_RCC_UART7_CLK_ENABLE();

      __HAL_RCC_GPIOF_CLK_ENABLE();
      /**UART7 GPIO Configuration
      PF6     ------> UART7_RX
      PF7     ------> UART7_TX
      */
      GPIO_InitStruct.Pin = GPIO_PIN_6|GPIO_PIN_7;
      GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
      GPIO_InitStruct.Pull = GPIO_NOPULL;
      GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
      GPIO_InitStruct.Alternate = GPIO_AF7_UART7;
      HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);

      /* UART7 DMA Init */
      /* UART7_RX Init */
      s_hdma_uart7_rx.Instance = DMA2_Stream0;
      s_hdma_uart7_rx.Init.Request = DMA_REQUEST_UART7_RX;
      s_hdma_uart7_rx.Init.Direction = DMA_PERIPH_TO_MEMORY;
      s_hdma_uart7_rx.Init.PeriphInc = DMA_PINC_DISABLE;
      s_hdma_uart7_rx.Init.MemInc = DMA_MINC_ENABLE;
      s_hdma_uart7_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
      s_hdma_uart7_rx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
      s_hdma_uart7_rx.Init.Mode = DMA_NORMAL;
      s_hdma_uart7_rx.Init.Priority = DMA_PRIORITY_LOW;
      s_hdma_uart7_rx.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
      if (HAL_DMA_Init(&s_hdma_uart7_rx) != HAL_OK)
      {
        error_handler();
      }

      __HAL_LINKDMA(uartHandle,hdmarx,s_hdma_uart7_rx);

      /* UART7_TX Init */
      s_hdma_uart7_tx.Instance = DMA2_Stream1;
      s_hdma_uart7_tx.Init.Request = DMA_REQUEST_UART7_TX;
      s_hdma_uart7_tx.Init.Direction = DMA_MEMORY_TO_PERIPH;
      s_hdma_uart7_tx.Init.PeriphInc = DMA_PINC_DISABLE;
      s_hdma_uart7_tx.Init.MemInc = DMA_MINC_ENABLE;
      s_hdma_uart7_tx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
      s_hdma_uart7_tx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
      s_hdma_uart7_tx.Init.Mode = DMA_NORMAL;
      s_hdma_uart7_tx.Init.Priority = DMA_PRIORITY_LOW;
      s_hdma_uart7_tx.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
      if (HAL_DMA_Init(&s_hdma_uart7_tx) != HAL_OK)
      {
        error_handler();
      }

      __HAL_LINKDMA(uartHandle,hdmatx,s_hdma_uart7_tx);

      HAL_NVIC_SetPriority(UART7_IRQn, 5, 0);
      HAL_NVIC_EnableIRQ(UART7_IRQn);

    }
}

void HAL_UART_MspDeInit(UART_HandleTypeDef* uartHandle)
{
  if(uartHandle->Instance==USART1)
  {
    __HAL_RCC_USART1_CLK_DISABLE();

    /**USART1 GPIO Configuration
    PA9     ------> USART1_TX
    PA10     ------> USART1_RX
    */
    HAL_GPIO_DeInit(GPIOA, UART_TX_DEBUG_Pin|UART_RX_DEBUG_Pin);
  }
  else if(uartHandle->Instance==UART7)
  {
    /* Peripheral clock disable */
    __HAL_RCC_UART7_CLK_DISABLE();

    /**UART7 GPIO Configuration
    PF6     ------> UART7_RX
    PF7     ------> UART7_TX
    */
    HAL_GPIO_DeInit(GPIOF, GPIO_PIN_6|GPIO_PIN_7);

    /* UART7 DMA DeInit */
    HAL_DMA_DeInit(uartHandle->hdmarx);
    HAL_DMA_DeInit(uartHandle->hdmatx);
  }
  else if(uartHandle->Instance==UART4)
  {
    /* Peripheral clock disable */
    __HAL_RCC_UART4_CLK_DISABLE();

    /**UART4 GPIO Configuration
    PI9     ------> UART4_RX
    PA0     ------> UART4_TX
    */
    HAL_GPIO_DeInit(GPIOI, GPIO_PIN_9);

    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_0);
  }
}

void DMA2_Stream0_IRQHandler(void)
{
  HAL_DMA_IRQHandler(&s_hdma_uart7_rx);
}

void DMA2_Stream1_IRQHandler(void)
{
  HAL_DMA_IRQHandler(&s_hdma_uart7_tx);
}

void UART7_IRQHandler(void)
{
  HAL_UART_IRQHandler(&g_uart7);
}



