/*
 * Copyright (c) 2026 beric-xiong
 * SPDX-License-Identifier: MIT
 *
 * ETH device abstraction layer
 */

#include <string.h>
#include <stdio.h>
#include "eth_dev.h"
#include "bsp_eth.h"
#include "pcf8574.h"
#include "eth_phy.h"
#include "delay_dev.h"
#include "log_print.h"
#include "main.h"

#include "FreeRTOS.h"
#include "semphr.h"
#include "task.h"

#define TAG "eth dev: "


#define ETH_RX_BUFFER_SIZE                     (1536UL)
#define ETH_RX_BUFFER_NUM                      (2 * ETH_RX_DESC_CNT)
#define ETH_DMA_TRANSMIT_TIMEOUT               (20U)

/* RX segment descriptor for frame chaining */
typedef struct RxSeg {
    uint8_t *data;
    uint16_t length;
    struct RxSeg *next;
} RxSeg;

/* RX buffer: segment descriptor + data buffer */
typedef struct {
    RxSeg seg;
    uint8_t buff[ETH_RX_BUFFER_SIZE];
} RxBuffer;

/* Private data structure */
typedef struct {
    ETH_HandleTypeDef *eth;
    unsigned char (*bsp_init)(uint8_t *mac_addr);
    uint8_t mac_addr[6];

    /* RX buffer pool */
    unsigned int rx_buff_free_mask;

    /* Callbacks */
    eth_rx_callback_t rx_cb;
    void *rx_cb_user_data;
    eth_link_callback_t link_cb;
    void *link_cb_user_data;

    /* TX config (persisted: Attributes/ChecksumCtrl/CRCPadCtrl set once in init) */
    ETH_TxPacketConfig tx_config;

    /* PHY */
    Eth_Chip_Object_T eth_chip;

    /* RTOS */
    SemaphoreHandle_t rx_semaphore;

    /* RX allocation status */
    volatile unsigned int rx_alloc_status;
} Eth_Data_T;

#define RX_ALLOC_OK     0U
#define RX_ALLOC_ERROR  1U

/* Function declarations */
static int eth_init(void *privatedata, uint8_t *mac_addr);
static int eth_deinit(void *privatedata);
static int eth_send(void *privatedata, uint8_t *data, uint16_t length);
static int eth_ioctl(void *privatedata, unsigned int cmd, void *arg);
static int eth_wait_rx(void *privatedata, unsigned int timeout_ms);
static int eth_register_rx_callback(void *privatedata, eth_rx_callback_t cb, void *user_data);
static int eth_register_link_callback(void *privatedata, eth_link_callback_t cb, void *user_data);
static int eth_start(void *privatedata);
static int eth_stop(void *privatedata);
static int eth_get_link_status(void *privatedata);

/* Buffer management */
static RxBuffer* rx_buff_alloc(Eth_Data_T *ed);
static void rx_buff_free(Eth_Data_T *ed, RxBuffer *buf);

/* PHY helpers */
static unsigned int eth_read_phy_reg(Eth_Data_T *ed, unsigned short reg_addr);
static void eth_phy_reset(void);

/* PHY IO callbacks */
static int eth_phy_io_init(void);
static int eth_phy_io_deinit(void);
static int eth_phy_io_read_reg(unsigned int dev_addr, unsigned int reg_addr, unsigned int *p_reg_val);
static int eth_phy_io_write_reg(unsigned int dev_addr, unsigned int reg_addr, unsigned int reg_val);
static int eth_phy_io_get_tick(void);

/* PHY IO function table */
static Eth_Chip_Ioctl_T eth_chip_io_ctx = {
    .init     = eth_phy_io_init,
    .deinit   = eth_phy_io_deinit,
    .writereg = eth_phy_io_write_reg,
    .readreg  = eth_phy_io_read_reg,
    .gettick  = eth_phy_io_get_tick
};

/* RX buffer pool - placed at dedicated MPU region */
__attribute__((section(".eth_rx_buf"))) static RxBuffer rx_buff_pool[ETH_RX_BUFFER_NUM];

/* RX completed frame queue */
static RxSeg *g_rx_completed_head = NULL;
static RxSeg *g_rx_completed_tail = NULL;
static RxSeg *g_current_frame = NULL;

/* Private data and device instances */
static Eth_Data_T s_eth1_data = {
    .eth = &g_eth_handler,
    .bsp_init = bsp_eth_init,
};

static Eth_Device_T s_eth1_dev = {
    .name = "eth1",
    .init = eth_init,
    .deinit = eth_deinit,
    .send = eth_send,
    .ioctl = eth_ioctl,
    .wait_rx = eth_wait_rx,
    .register_rx_callback = eth_register_rx_callback,
    .register_link_callback = eth_register_link_callback,
    .start = eth_start,
    .stop = eth_stop,
    .get_link_status = eth_get_link_status,
    .private_data = &s_eth1_data,
};

/* ---- Buffer pool management ---- */

static RxBuffer* rx_buff_alloc(Eth_Data_T *ed)
{
    unsigned int primask;
    unsigned int idx;

    primask = __get_PRIMASK();
    __disable_irq();

    for (idx = 0; idx < ETH_RX_BUFFER_NUM; idx++)
    {
        if (ed->rx_buff_free_mask & (1U << idx))
        {
            ed->rx_buff_free_mask &= ~(1U << idx);
            if (!primask)
                __enable_irq();
            return &rx_buff_pool[idx];
        }
    }

    if (!primask)
        __enable_irq();
    return NULL;
}

static void rx_buff_free(Eth_Data_T *ed, RxBuffer *buf)
{
    int idx = buf - rx_buff_pool;
    __disable_irq();
    ed->rx_buff_free_mask |= (1U << idx);
    __enable_irq();
}

/* ---- PHY helpers ---- */

static unsigned int eth_read_phy_reg(Eth_Data_T *ed, unsigned short reg_addr)
{
    uint32_t regval;

    HAL_ETH_ReadPHYRegister(ed->eth, ETH_CHIP_ADDR, reg_addr, &regval);
    return regval;
}


static void sys_intx_disable(void)
{
    __ASM volatile("cpsid i");
}

static void sys_intx_enable(void)
{
    __ASM volatile("cpsie i");
}

static void eth_phy_reset(void)
{
    unsigned int regval;

    sys_intx_disable();

    regval = eth_read_phy_reg(&s_eth1_data, 2);

    if ((regval & 0xFFF) == 0xFFF)
    {
        pcf8574_write_bit(ETH_RESET_IO, 1);
        delay_ms(100);
        pcf8574_write_bit(ETH_RESET_IO, 0);
        delay_ms(100);
    }
    else
    {
        pcf8574_write_bit(ETH_RESET_IO, 0);
        delay_ms(100);
        pcf8574_write_bit(ETH_RESET_IO, 1);
        delay_ms(100);
    }

    sys_intx_enable();
}

/* ---- Eth_Device_T interface implementations ---- */

static int eth_init(void *privatedata, uint8_t *mac_addr)
{
    if (privatedata == NULL) {
        LOG_PRINT(LOG_OUT_ERROR, "%sETH init private data is null!\n", TAG);
        return -1;
    }

    Eth_Device_T *eth_dev = (Eth_Device_T *)privatedata;
    Eth_Data_T *ed = (Eth_Data_T *)eth_dev->private_data;

    if (ed == NULL) {
        LOG_PRINT(LOG_OUT_ERROR, "%sETH data is null!\n", TAG);
        return -1;
    }

    memcpy(ed->mac_addr, mac_addr, 6);

    /* BSP hardware init (HAL_ETH_Init, GPIOs, clocks, MPU) */
    ed->bsp_init(ed->mac_addr);

    /* MDIO clock must be configured before any PHY register access */
    HAL_ETH_SetMDIOClockRange(&g_eth_handler);

    /* PHY reset */
    eth_phy_reset();

    /* Init TX config */
    memset(&ed->tx_config, 0, sizeof(ETH_TxPacketConfig));
    ed->tx_config.Attributes = ETH_TX_PACKETS_FEATURES_CSUM | ETH_TX_PACKETS_FEATURES_CRCPAD;
    ed->tx_config.ChecksumCtrl = ETH_CHECKSUM_IPHDR_PAYLOAD_INSERT_PHDR_CALC;
    ed->tx_config.CRCPadCtrl = ETH_CRC_PAD_INSERT;

    /* Init RX buffer pool */
    ed->rx_buff_free_mask = (1U << ETH_RX_BUFFER_NUM) - 1;

    /* Create RX semaphore */
    ed->rx_semaphore = xSemaphoreCreateBinary();
    if (ed->rx_semaphore == NULL) {
        LOG_PRINT(LOG_OUT_ERROR, "%sETH semaphore create failed!\n", TAG);
        return -1;
    }

    /* Register PHY IO functions */
    eth_phy_register_bus_io(&ed->eth_chip, &eth_chip_io_ctx);

    /* Init PHY */
    eth_phy_init(&ed->eth_chip);

    /* Start auto-negotiation */
    eth_phy_start_auto_nego(&ed->eth_chip);

    delay_ms(2000);

    int phy_link_state = eth_phy_get_link_state(&ed->eth_chip);
    if (phy_link_state == ETH_CHIP_STATUS_READ_ERROR)
    {
        LOG_PRINT(LOG_OUT_ERROR, "%sPHY link state read error!\n", TAG);
        return -1;
    }

    unsigned int duplex, speed;
    switch (phy_link_state)
    {
        case ETH_CHIP_STATUS_100MBITS_FULLDUPLEX:
            duplex = ETH_FULLDUPLEX_MODE;
            speed = ETH_SPEED_100M;
            break;
        case ETH_CHIP_STATUS_100MBITS_HALFDUPLEX:
            duplex = ETH_HALFDUPLEX_MODE;
            speed = ETH_SPEED_100M;
            break;
        case ETH_CHIP_STATUS_10MBITS_FULLDUPLEX:
            duplex = ETH_FULLDUPLEX_MODE;
            speed = ETH_SPEED_10M;
            break;
        case ETH_CHIP_STATUS_10MBITS_HALFDUPLEX:
            duplex = ETH_HALFDUPLEX_MODE;
            speed = ETH_SPEED_10M;
            break;
        default:
            duplex = ETH_FULLDUPLEX_MODE;
            speed = ETH_SPEED_100M;
            break;
    }

    /* Configure MAC */
    ETH_MACConfigTypeDef mac_config;
    HAL_ETH_GetMACConfig(ed->eth, &mac_config);
    mac_config.DuplexMode = duplex;
    mac_config.Speed = speed;
    HAL_ETH_SetMACConfig(ed->eth, &mac_config);

    /* Start ETH DMA in interrupt mode */
    HAL_ETH_Start_IT(ed->eth);

    /* Verify MCU-PHY communication */
    {
        int phy_check_timeout = 1000;
        while (!eth_read_phy_reg(ed, ed->eth_chip.physcsr) && phy_check_timeout > 0)
        {
            delay_ms(1);
            phy_check_timeout--;
        }
        if (phy_check_timeout <= 0)
        {
            LOG_PRINT(LOG_OUT_ERROR, "%sMCU-PHY communication failed!\n", TAG);
        }
    }

    /* Notify link up via callback */
    if (ed->link_cb) {
        ed->link_cb(ed->link_cb_user_data, 1, speed, duplex);
    }

    LOG_PRINT(LOG_OUT_DEBUG, "%sETH init successful!\n", TAG);
    return 0;
}

static int eth_deinit(void *privatedata)
{
    if (privatedata == NULL) {
        return -1;
    }

    Eth_Device_T *eth_dev = (Eth_Device_T *)privatedata;
    Eth_Data_T *ed = (Eth_Data_T *)eth_dev->private_data;

    HAL_ETH_Stop_IT(ed->eth);
    eth_phy_deinit(&ed->eth_chip);

    if (ed->rx_semaphore) {
        vSemaphoreDelete(ed->rx_semaphore);
        ed->rx_semaphore = NULL;
    }

    return 0;
}

static int eth_send(void *privatedata, uint8_t *data, uint16_t length)
{
    if (privatedata == NULL) {
        LOG_PRINT(LOG_OUT_ERROR, "%sETH send private data is null!\n", TAG);
        return -1;
    }

    Eth_Device_T *eth_dev = (Eth_Device_T *)privatedata;
    Eth_Data_T *ed = (Eth_Data_T *)eth_dev->private_data;

    ETH_BufferTypeDef Txbuffer[ETH_TX_DESC_CNT];
    memset(Txbuffer, 0, sizeof(Txbuffer));

    Txbuffer[0].buffer = data;
    Txbuffer[0].len = length;
    Txbuffer[0].next = NULL;

    ed->tx_config.Length = length;
    ed->tx_config.TxBuffer = Txbuffer;

    if (HAL_ETH_Transmit(ed->eth, &ed->tx_config, ETH_DMA_TRANSMIT_TIMEOUT) != HAL_OK)
    {
        LOG_PRINT(LOG_OUT_ERROR, "%sETH send failed!\n", TAG);
        return -1;
    }

    return 0;
}

static int eth_ioctl(void *privatedata, unsigned int cmd, void *arg)
{
    (void)cmd;
    (void)arg;

    if (privatedata == NULL) {
        return -1;
    }

    return 0;
}

static int eth_wait_rx(void *privatedata, unsigned int timeout_ms)
{
    if (privatedata == NULL)
        return -1;

    Eth_Device_T *eth_dev = (Eth_Device_T *)privatedata;
    Eth_Data_T *ed = (Eth_Data_T *)eth_dev->private_data;

    if (xSemaphoreTake(ed->rx_semaphore, timeout_ms) != pdTRUE)
    {
        return 0;
    }

    int count = 0;

    for (;;)
    {
        taskENTER_CRITICAL();
        RxSeg *frame = g_rx_completed_head;
        if (frame)
        {
            g_rx_completed_head = frame->next;
            if (g_rx_completed_head == NULL)
            {
                g_rx_completed_tail = NULL;
            }
            frame->next = NULL;
        }
        taskEXIT_CRITICAL();

        if (!frame)
            break;

        /* Call rx_callback for each segment in the frame */
        if (ed->rx_cb)
        {
            RxSeg *seg = frame;
            while (seg)
            {
                ed->rx_cb(ed->rx_cb_user_data, seg->data, seg->length);
                seg = seg->next;
            }
        }

        /* Free all segments back to pool */
        RxSeg *seg = frame;
        while (seg)
        {
            RxBuffer *rx_buf = (RxBuffer *)((uint8_t *)seg - offsetof(RxBuffer, seg));
            RxSeg *next = seg->next;
            rx_buff_free(ed, rx_buf);
            seg = next;
        }

        count++;
    }

    return count;
}

static int eth_register_rx_callback(void *privatedata, eth_rx_callback_t cb, void *user_data)
{
    if (privatedata == NULL)
        return -1;

    Eth_Device_T *eth_dev = (Eth_Device_T *)privatedata;
    Eth_Data_T *ed = (Eth_Data_T *)eth_dev->private_data;

    ed->rx_cb = cb;
    ed->rx_cb_user_data = user_data;

    return 0;
}

static int eth_register_link_callback(void *privatedata, eth_link_callback_t cb, void *user_data)
{
    if (privatedata == NULL)
        return -1;

    Eth_Device_T *eth_dev = (Eth_Device_T *)privatedata;
    Eth_Data_T *ed = (Eth_Data_T *)eth_dev->private_data;

    ed->link_cb = cb;
    ed->link_cb_user_data = user_data;

    return 0;
}

static int eth_start(void *privatedata)
{
    if (privatedata == NULL) {
        return -1;
    }

    Eth_Device_T *eth_dev = (Eth_Device_T *)privatedata;
    Eth_Data_T *ed = (Eth_Data_T *)eth_dev->private_data;

    if (ed == NULL || ed->eth == NULL) {
        return -1;
    }

    HAL_ETH_Start_IT(ed->eth);
    return 0;
}

static int eth_stop(void *privatedata)
{
    if (privatedata == NULL) {
        return -1;
    }

    Eth_Device_T *eth_dev = (Eth_Device_T *)privatedata;
    Eth_Data_T *ed = (Eth_Data_T *)eth_dev->private_data;

    if (ed == NULL || ed->eth == NULL) {
        return -1;
    }

    HAL_ETH_Stop_IT(ed->eth);
    return 0;
}

static int eth_get_link_status(void *privatedata)
{
    if (privatedata == NULL) {
        return -1;
    }

    Eth_Device_T *eth_dev = (Eth_Device_T *)privatedata;
    Eth_Data_T *ed = (Eth_Data_T *)eth_dev->private_data;

    if (ed == NULL || ed->eth == NULL) {
        return -1;
    }

    uint32_t regval;
    HAL_ETH_ReadPHYRegister(ed->eth, ETH_CHIP_ADDR, ETH_CHIP_BSR, &regval);
    return (regval & ETH_CHIP_BSR_LINK_STATUS) ? 1 : 0;
}

/* ---- HAL ETH Callbacks ---- */

void HAL_ETH_RxCpltCallback(ETH_HandleTypeDef *heth)
{
    if (heth == s_eth1_data.eth && g_current_frame)
    {
        /* Add current frame to completed queue */
        if (g_rx_completed_tail)
        {
            g_rx_completed_tail->next = g_current_frame;
        }
        else
        {
            g_rx_completed_head = g_current_frame;
        }

        /* Advance tail to end of chain */
        RxSeg *s = g_current_frame;
        while (s->next)
        {
            s = s->next;
        }
        g_rx_completed_tail = s;
        g_current_frame = NULL;

        /* Signal RX task */
        portBASE_TYPE taskWoken = pdFALSE;
        if (xSemaphoreGiveFromISR(s_eth1_data.rx_semaphore, &taskWoken) == pdTRUE)
        {
            portEND_SWITCHING_ISR(taskWoken);
        }
    }
}

void HAL_ETH_RxAllocateCallback(uint8_t **buff)
{
    if (s_eth1_data.eth == NULL)
    {
        *buff = NULL;
        return;
    }

    RxBuffer *rx_buf = rx_buff_alloc(&s_eth1_data);
    if (rx_buf)
    {
        s_eth1_data.rx_alloc_status = RX_ALLOC_OK;
        *buff = rx_buf->buff;
    }
    else
    {
        s_eth1_data.rx_alloc_status = RX_ALLOC_ERROR;
        *buff = NULL;
    }
}

void HAL_ETH_RxLinkCallback(void **pStart, void **pEnd, uint8_t *buff, uint16_t length)
{
    /* Locate RxBuffer from buff pointer */
    RxBuffer *rx_buf = (RxBuffer *)(buff - offsetof(RxBuffer, buff));
    RxSeg *seg = &rx_buf->seg;

    seg->data = buff;
    seg->length = length;
    seg->next = NULL;

    /* Chain using HAL's pStart/pEnd mechanism */
    if (*pStart == NULL)
    {
        *pStart = seg;
        g_current_frame = seg;
    }
    else
    {
        ((RxSeg *)(*pEnd))->next = seg;
    }
    *pEnd = seg;

    /* Invalidate data cache for DMA-coherent data */
    SCB_InvalidateDCache_by_Addr((unsigned int *)buff, length);
}

void HAL_ETH_ErrorCallback(ETH_HandleTypeDef *heth)
{
    if (heth == s_eth1_data.eth)
    {
        LOG_PRINT(LOG_OUT_ERROR, "%sETH error callback!\n", TAG);
    }
}

/* ---- PHY IO Functions ---- */

static int eth_phy_io_init(void)
{
    return 0;
}

static int eth_phy_io_deinit(void)
{
    return 0;
}

static int eth_phy_io_read_reg(unsigned int dev_addr, unsigned int reg_addr, unsigned int *p_reg_val)
{
    if (HAL_ETH_ReadPHYRegister(&g_eth_handler, dev_addr, reg_addr, (uint32_t *)p_reg_val) != HAL_OK)
    {
        return -1;
    }
    return 0;
}

static int eth_phy_io_write_reg(unsigned int dev_addr, unsigned int reg_addr, unsigned int reg_val)
{
    if (HAL_ETH_WritePHYRegister(&g_eth_handler, dev_addr, reg_addr, reg_val) != HAL_OK)
    {
        return -1;
    }
    return 0;
}

static int eth_phy_io_get_tick(void)
{
    return HAL_GetTick();
}

/* ---- Public API ---- */

Eth_Device_T *get_eth_device(char *name)
{
    if (0 == strcmp(name, "eth1"))
        return &s_eth1_dev;

    return NULL;
}
