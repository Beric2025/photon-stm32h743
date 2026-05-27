/*
 * Copyright (c) 2026 beric-xiong
 * SPDX-License-Identifier: MIT
 *
 * QSPI device abstraction layer
 */

#include <string.h>
#include <stdio.h>
#include "main.h"
#include "qspi_dev.h"
#include "bsp_qspi.h"
#include "log_print.h"

#define TAG "qspi dev:"

typedef struct {
    QSPI_HandleTypeDef *hqspi;
    void (*init)(void);
} Qspi_Data_T;

/* function declarations */
static int qspi_init(void *privatedata);
static int qspi_cmd(void *privatedata, unsigned char instr, unsigned int addr,
                    unsigned char *buf, unsigned int len);
static int qspi_read(void *privatedata, unsigned int addr, unsigned char *buf,
                     unsigned int len);
static int qspi_write(void *privatedata, unsigned int addr, unsigned char *buf,
                      unsigned int len);

/* QSPI hardware data */
static Qspi_Data_T s_qspi_data = {
    .hqspi = &g_qspi,
    .init = bsp_qspi_init,
};

/* QSPI device instance */
static Qspi_Device_T s_qspi_dev = {
    .name = "qspi",
    .init = qspi_init,
    .cmd = qspi_cmd,
    .read = qspi_read,
    .write = qspi_write,
    .private_data = &s_qspi_data,
};


static int qspi_init(void *privatedata)
{
    if (privatedata == NULL) {
        LOG_PRINT(LOG_OUT_ERROR, "%s QSPI init private data is null!\n", TAG);
        return -1;
    }

    Qspi_Device_T *qspi_dev = (Qspi_Device_T *)privatedata;
    Qspi_Data_T *qspi_data = (Qspi_Data_T *)qspi_dev->private_data;

    qspi_data->init();

    LOG_PRINT(LOG_OUT_DEBUG, "%s QSPI init successful!\n", TAG);

    return 0;
}

static int qspi_cmd(void *privatedata, unsigned char instr, unsigned int addr,
                    unsigned char *buf, unsigned int len)
{
    if (privatedata == NULL || buf == NULL || len == 0)
        return -1;

    Qspi_Device_T *qspi_dev = (Qspi_Device_T *)privatedata;
    Qspi_Data_T *qspi_data = (Qspi_Data_T *)qspi_dev->private_data;

    QSPI_CommandTypeDef sCmd = {0};
    sCmd.Instruction = instr;
    sCmd.Address = addr;
    sCmd.AddressSize = QSPI_ADDRESS_24_BITS;
    sCmd.DummyCycles = 0;
    sCmd.InstructionMode = QSPI_INSTRUCTION_1_LINE;
    sCmd.AddressMode = QSPI_ADDRESS_1_LINE;
    sCmd.DataMode = QSPI_DATA_1_LINE;
    sCmd.NbData = len;
    sCmd.DdrMode = QSPI_DDR_MODE_DISABLE;
    sCmd.SIOOMode = QSPI_SIOO_INST_ONLY_FIRST_CMD;

    if (HAL_QSPI_Command(qspi_data->hqspi, &sCmd, 5000) != HAL_OK)
        return -1;
    if (HAL_QSPI_Transmit(qspi_data->hqspi, buf, 5000) != HAL_OK)
        return -1;

    return 0;
}

static int qspi_read(void *privatedata, unsigned int addr, unsigned char *buf,
                     unsigned int len)
{
    if (privatedata == NULL || buf == NULL || len == 0)
        return -1;

    Qspi_Device_T *qspi_dev = (Qspi_Device_T *)privatedata;
    Qspi_Data_T *qspi_data = (Qspi_Data_T *)qspi_dev->private_data;

    QSPI_CommandTypeDef sCmd = {0};
    sCmd.Instruction = 0x03;   /* standard read */
    sCmd.Address = addr;
    sCmd.AddressSize = QSPI_ADDRESS_24_BITS;
    sCmd.DummyCycles = 0;
    sCmd.InstructionMode = QSPI_INSTRUCTION_1_LINE;
    sCmd.AddressMode = QSPI_ADDRESS_1_LINE;
    sCmd.DataMode = QSPI_DATA_1_LINE;
    sCmd.NbData = len;
    sCmd.DdrMode = QSPI_DDR_MODE_DISABLE;
    sCmd.SIOOMode = QSPI_SIOO_INST_ONLY_FIRST_CMD;

    if (HAL_QSPI_Command(qspi_data->hqspi, &sCmd, 5000) != HAL_OK)
        return -1;
    if (HAL_QSPI_Receive(qspi_data->hqspi, buf, 5000) != HAL_OK)
        return -1;

    return 0;
}

static int qspi_write(void *privatedata, unsigned int addr, unsigned char *buf,
                      unsigned int len)
{
    if (privatedata == NULL || buf == NULL || len == 0)
        return -1;

    Qspi_Device_T *qspi_dev = (Qspi_Device_T *)privatedata;
    Qspi_Data_T *qspi_data = (Qspi_Data_T *)qspi_dev->private_data;

    /* Write enable */
    QSPI_CommandTypeDef sCmd = {0};
    sCmd.Instruction = 0x06;   /* write enable */
    sCmd.InstructionMode = QSPI_INSTRUCTION_1_LINE;
    sCmd.DdrMode = QSPI_DDR_MODE_DISABLE;
    sCmd.SIOOMode = QSPI_SIOO_INST_ONLY_FIRST_CMD;
    if (HAL_QSPI_Command(qspi_data->hqspi, &sCmd, 5000) != HAL_OK)
        return -1;

    /* Page program */
    sCmd.Instruction = 0x02;   /* page program */
    sCmd.Address = addr;
    sCmd.AddressSize = QSPI_ADDRESS_24_BITS;
    sCmd.DummyCycles = 0;
    sCmd.AddressMode = QSPI_ADDRESS_1_LINE;
    sCmd.DataMode = QSPI_DATA_1_LINE;
    sCmd.NbData = len;
    if (HAL_QSPI_Command(qspi_data->hqspi, &sCmd, 5000) != HAL_OK)
        return -1;
    if (HAL_QSPI_Transmit(qspi_data->hqspi, buf, 5000) != HAL_OK)
        return -1;

    return 0;
}

Qspi_Device_T *get_qspi_device(char *name)
{
    Qspi_Device_T *qspi_dev = NULL;

    if (0 == strcmp(name, "qspi"))
        qspi_dev = &s_qspi_dev;

    return qspi_dev;
}
