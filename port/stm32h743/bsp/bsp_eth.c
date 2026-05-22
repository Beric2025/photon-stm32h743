/*
 * Copyright (c) 2026 beric-xiong
 * SPDX-License-Identifier: MIT
 *
 * ETH low-level driver interface
 */

#include "bsp_eth.h"

ETH_HandleTypeDef   g_eth_handler;
#if defined ( __ICCARM__ ) /*!< IAR Compiler */

#pragma location=0x30040000
ETH_DMADescTypeDef  g_ethDmaRxDscrTab[ETH_RX_DESC_CNT];
#pragma location=0x30040060
ETH_DMADescTypeDef  g_ethDmaTxDscrTab[ETH_TX_DESC_CNT];

#elif defined ( __CC_ARM )  /* MDK ARM Compiler */

__attribute__((at(0x30040000))) ETH_DMADescTypeDef  g_ethDmaRxDscrTab[ETH_RX_DESC_CNT];
__attribute__((at(0x30040060))) ETH_DMADescTypeDef  g_ethDmaTxDscrTab[ETH_TX_DESC_CNT];

#elif defined ( __GNUC__ ) /* GNU Compiler */

ETH_DMADescTypeDef g_ethDmaRxDscrTab[ETH_RX_DESC_CNT] __attribute__((section(".RxDescripSection")));
ETH_DMADescTypeDef g_ethDmaTxDscrTab[ETH_TX_DESC_CNT] __attribute__((section(".TxDescripSection")));

#endif

static void net_mpu_config(void)
{
    MPU_Region_InitTypeDef MPU_InitStruct;

    HAL_MPU_Disable();
    MPU_InitStruct.Enable = MPU_REGION_ENABLE;
    MPU_InitStruct.BaseAddress = 0x30040000;
    MPU_InitStruct.Size = MPU_REGION_SIZE_16KB;
    MPU_InitStruct.AccessPermission = MPU_REGION_FULL_ACCESS;
    MPU_InitStruct.IsBufferable = MPU_ACCESS_BUFFERABLE;
    MPU_InitStruct.IsCacheable = MPU_ACCESS_NOT_CACHEABLE;
    MPU_InitStruct.IsShareable = MPU_ACCESS_SHAREABLE;
    MPU_InitStruct.Number = MPU_REGION_NUMBER5;
    MPU_InitStruct.TypeExtField = MPU_TEX_LEVEL0;
    MPU_InitStruct.SubRegionDisable = 0x00;
    MPU_InitStruct.DisableExec = MPU_INSTRUCTION_ACCESS_ENABLE;
    HAL_MPU_ConfigRegion(&MPU_InitStruct);
    HAL_MPU_Enable(MPU_PRIVILEGED_DEFAULT);
}


/**
 * bsp_eth_init - Ethernet chip initialization
 * @mac_addr:  MAC address array (6 bytes)
 *
 * Return: 0 on success, 1 on failure
 */
unsigned char bsp_eth_init(uint8_t *mac_addr)
{
    net_mpu_config();

    g_eth_handler.Instance = ETH;
    g_eth_handler.Init.MACAddr = mac_addr;
    g_eth_handler.Init.MediaInterface = HAL_ETH_RMII_MODE;
    g_eth_handler.Init.RxDesc = g_ethDmaRxDscrTab;
    g_eth_handler.Init.TxDesc = g_ethDmaTxDscrTab;
    g_eth_handler.Init.RxBuffLen = ETH_MAX_PACKET_SIZE;

    if (HAL_ETH_Init(&g_eth_handler) == HAL_OK)
    {
        return 0;
    }
    else
    {
        return 1;
    }
}

void HAL_ETH_MspInit(ETH_HandleTypeDef *heth)
{
    (void)heth;
    GPIO_InitTypeDef gpio_init_struct;

    ETH_CLK_GPIO_CLK_ENABLE();
    ETH_MDIO_GPIO_CLK_ENABLE();
    ETH_CRS_GPIO_CLK_ENABLE();
    ETH_MDC_GPIO_CLK_ENABLE();
    ETH_RXD0_GPIO_CLK_ENABLE();
    ETH_RXD1_GPIO_CLK_ENABLE();
    ETH_TX_EN_GPIO_CLK_ENABLE();
    ETH_TXD0_GPIO_CLK_ENABLE();
    ETH_TXD1_GPIO_CLK_ENABLE();

    __HAL_RCC_ETH1MAC_CLK_ENABLE();
    __HAL_RCC_ETH1TX_CLK_ENABLE();
    __HAL_RCC_ETH1RX_CLK_ENABLE();

    /* Network pin configuration - RMII interface
     * ETH_MDIO -------------------------> PA2
     * ETH_MDC --------------------------> PC1
     * ETH_RMII_REF_CLK------------------> PA1
     * ETH_RMII_CRS_DV ------------------> PA7
     * ETH_RMII_RXD0 --------------------> PC4
     * ETH_RMII_RXD1 --------------------> PC5
     * ETH_RMII_TX_EN -------------------> PB11
     * ETH_RMII_TXD0 --------------------> PG13
     * ETH_RMII_TXD1 --------------------> PG14
     */

    gpio_init_struct.Pin = ETH_CLK_GPIO_PIN;
    gpio_init_struct.Mode = GPIO_MODE_AF_PP;
    gpio_init_struct.Pull = GPIO_NOPULL;
    gpio_init_struct.Speed = GPIO_SPEED_HIGH;
    gpio_init_struct.Alternate = GPIO_AF11_ETH;
    HAL_GPIO_Init(ETH_CLK_GPIO_PORT, &gpio_init_struct);

    gpio_init_struct.Pin = ETH_MDIO_GPIO_PIN;
    HAL_GPIO_Init(ETH_MDIO_GPIO_PORT, &gpio_init_struct);

    gpio_init_struct.Pin = ETH_CRS_GPIO_PIN;
    HAL_GPIO_Init(ETH_CRS_GPIO_PORT, &gpio_init_struct);

    gpio_init_struct.Pin = ETH_MDC_GPIO_PIN;
    HAL_GPIO_Init(ETH_MDC_GPIO_PORT, &gpio_init_struct);

    gpio_init_struct.Pin = ETH_RXD0_GPIO_PIN;
    HAL_GPIO_Init(ETH_RXD0_GPIO_PORT, &gpio_init_struct);

    gpio_init_struct.Pin = ETH_RXD1_GPIO_PIN;
    HAL_GPIO_Init(ETH_RXD1_GPIO_PORT, &gpio_init_struct);


    gpio_init_struct.Pin = ETH_TX_EN_GPIO_PIN;
    HAL_GPIO_Init(ETH_TX_EN_GPIO_PORT, &gpio_init_struct);

    gpio_init_struct.Pin = ETH_TXD0_GPIO_PIN;
    HAL_GPIO_Init(ETH_TXD0_GPIO_PORT, &gpio_init_struct);

    gpio_init_struct.Pin = ETH_TXD1_GPIO_PIN;
    HAL_GPIO_Init(ETH_TXD1_GPIO_PORT, &gpio_init_struct);

    HAL_NVIC_SetPriority(ETH_IRQn, 0x07, 0);
    HAL_NVIC_EnableIRQ(ETH_IRQn);
}

void HAL_ETH_MspDeInit(ETH_HandleTypeDef *heth)
{
    (void)heth;
    __HAL_RCC_ETH1MAC_CLK_DISABLE();
    __HAL_RCC_ETH1TX_CLK_DISABLE();
    __HAL_RCC_ETH1RX_CLK_DISABLE();

    HAL_GPIO_DeInit(ETH_CLK_GPIO_PORT, ETH_CLK_GPIO_PIN);
    HAL_GPIO_DeInit(ETH_MDIO_GPIO_PORT, ETH_MDIO_GPIO_PIN);
    HAL_GPIO_DeInit(ETH_CRS_GPIO_PORT, ETH_CRS_GPIO_PIN);
    HAL_GPIO_DeInit(ETH_MDC_GPIO_PORT, ETH_MDC_GPIO_PIN);
    HAL_GPIO_DeInit(ETH_RXD0_GPIO_PORT, ETH_RXD0_GPIO_PIN);
    HAL_GPIO_DeInit(ETH_RXD1_GPIO_PORT, ETH_RXD1_GPIO_PIN);
    HAL_GPIO_DeInit(ETH_TX_EN_GPIO_PORT, ETH_TX_EN_GPIO_PIN);
    HAL_GPIO_DeInit(ETH_TXD0_GPIO_PORT, ETH_TXD0_GPIO_PIN);
    HAL_GPIO_DeInit(ETH_TXD1_GPIO_PORT, ETH_TXD1_GPIO_PIN);
}

void ETH_IRQHandler(void)
{
    HAL_ETH_IRQHandler(&g_eth_handler);
}
