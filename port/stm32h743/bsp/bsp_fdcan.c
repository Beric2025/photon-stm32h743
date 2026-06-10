/*
 * Copyright (c) 2026 beric-xiong
 * SPDX-License-Identifier: MIT
 *
 * FDCAN board support package
 */

#include <string.h>
#include <stdio.h>
#include "bsp_fdcan.h"
#include "can_dev.h"

FDCAN_HandleTypeDef   g_fdcan1;
FDCAN_TxHeaderTypeDef g_fdcan1_tx;
FDCAN_RxHeaderTypeDef g_fdcan1_rx;

unsigned char bsp_fdcan1_init(unsigned short presc, unsigned char tsjw,
    unsigned short ntsg1, unsigned char ntsg2, uint32_t mode)
{
    FDCAN_FilterTypeDef fdcan_filterconfig;

    HAL_FDCAN_DeInit(&g_fdcan1);
    g_fdcan1.Instance = FDCAN1;
    g_fdcan1.Init.FrameFormat = FDCAN_FRAME_CLASSIC;
    g_fdcan1.Init.Mode = mode;
    g_fdcan1.Init.AutoRetransmission = DISABLE;               /* must be off in classic mode */
    g_fdcan1.Init.TransmitPause = DISABLE;
    g_fdcan1.Init.ProtocolException = DISABLE;
    g_fdcan1.Init.NominalPrescaler = presc;
    g_fdcan1.Init.NominalSyncJumpWidth = tsjw;
    g_fdcan1.Init.NominalTimeSeg1 = ntsg1;
    g_fdcan1.Init.NominalTimeSeg2 = ntsg2;
    g_fdcan1.Init.MessageRAMOffset = 0;
    g_fdcan1.Init.StdFiltersNbr = 0;
    g_fdcan1.Init.ExtFiltersNbr = 0;
    g_fdcan1.Init.RxFifo0ElmtsNbr = 1;
    g_fdcan1.Init.RxFifo0ElmtSize = FDCAN_DATA_BYTES_8;
    g_fdcan1.Init.RxBuffersNbr = 0;
    g_fdcan1.Init.TxEventsNbr = 0;
    g_fdcan1.Init.TxBuffersNbr = 0;
    g_fdcan1.Init.TxFifoQueueElmtsNbr = 1;
    g_fdcan1.Init.TxFifoQueueMode = FDCAN_TX_FIFO_OPERATION;
    g_fdcan1.Init.TxElmtSize = FDCAN_DATA_BYTES_8;

    if (HAL_FDCAN_Init(&g_fdcan1) != HAL_OK)
    {
        return 1;
    }

    fdcan_filterconfig.IdType = FDCAN_STANDARD_ID;
    fdcan_filterconfig.FilterIndex = 0;
    fdcan_filterconfig.FilterType = FDCAN_FILTER_MASK;
    fdcan_filterconfig.FilterConfig = FDCAN_FILTER_TO_RXFIFO0;
    fdcan_filterconfig.FilterID1 = 0x0000;
    fdcan_filterconfig.FilterID2 = 0x0000;

    if (HAL_FDCAN_ConfigFilter(&g_fdcan1, &fdcan_filterconfig) != HAL_OK)
    {
        return 2;
    }

    HAL_FDCAN_Start(&g_fdcan1);
    HAL_FDCAN_ActivateNotification(&g_fdcan1, FDCAN_IT_RX_FIFO0_NEW_MESSAGE, 0);

    return 0;
}

void HAL_FDCAN_MspInit(FDCAN_HandleTypeDef *hcan)
{
    if (FDCAN1 == hcan->Instance)
    {
        GPIO_InitTypeDef gpio_init_struct;
        RCC_PeriphCLKInitTypeDef fdcan_periphclk;

        __HAL_RCC_FDCAN_CLK_ENABLE();
        __HAL_RCC_GPIOH_CLK_ENABLE();

        /* Set FDCAN1 clock source to PLL1Q */
        fdcan_periphclk.PeriphClockSelection = RCC_PERIPHCLK_FDCAN;
        fdcan_periphclk.FdcanClockSelection = RCC_FDCANCLKSOURCE_PLL;
        HAL_RCCEx_PeriphCLKConfig(&fdcan_periphclk);

        gpio_init_struct.Pin = GPIO_PIN_13 | GPIO_PIN_14;
        gpio_init_struct.Mode = GPIO_MODE_AF_PP;
        gpio_init_struct.Pull = GPIO_PULLUP;
        gpio_init_struct.Speed = GPIO_SPEED_FAST;
        gpio_init_struct.Alternate = GPIO_AF9_FDCAN1;               /* AF9: FDCAN1 */
        HAL_GPIO_Init(GPIOH, &gpio_init_struct);

#if FDCAN1_RX0_INT_ENABLE
        HAL_NVIC_SetPriority(FDCAN1_IT0_IRQn, 5, 0);
        HAL_NVIC_EnableIRQ(FDCAN1_IT0_IRQn);
#endif
    }
}

void HAL_FDCAN_MspDeInit(FDCAN_HandleTypeDef *hfdcan)
{
    (void)hfdcan;
    __HAL_RCC_FDCAN_FORCE_RESET();
    __HAL_RCC_FDCAN_RELEASE_RESET();

#if FDCAN1_RX0_INT_ENABLE
    HAL_NVIC_DisableIRQ(FDCAN1_IT0_IRQn);
#endif
}

/* ====================================================================
 * HAL callbacks — translates to generic event for interface layer
 * ==================================================================== */

#if FDCAN1_RX0_INT_ENABLE

void FDCAN1_IT0_IRQHandler(void)
{
    HAL_FDCAN_IRQHandler(&g_fdcan1);
}

void HAL_FDCAN_RxFifo0Callback(FDCAN_HandleTypeDef *hfdcan, uint32_t RxFifo0ITs)
{
    (void)RxFifo0ITs;
    can_dev_on_event((void *)hfdcan, CAN_EVENT_RX_FIFO0);
}

#endif
