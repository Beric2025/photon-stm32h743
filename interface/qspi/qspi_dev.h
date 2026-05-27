/*
 * Copyright (c) 2026 beric-xiong
 * SPDX-License-Identifier: MIT
 */

#ifndef _QSPI_DEV_H_
#define _QSPI_DEV_H_

#ifdef __cplusplus
extern "C" {
#endif


typedef struct {
    /* device identifier (e.g. "qspi") */
    char *name;

    /**
     * @brief:  initialize QSPI hardware
     * @privatedata:  pointer to this Qspi_Device_T
     *
     * Return: 0 on success, -1 on failure
     */
    int (*init)(void *privatedata);

    /**
     * @brief:  send command with address and data phases
     * @privatedata:  pointer to this Qspi_Device_T
     * @instr:        instruction byte (e.g. 0x03 read, 0x02 write)
     * @addr:         target address
     * @buf:          data buffer (tx or rx)
     * @len:          number of bytes to transfer
     *
     * Return: 0 on success, -1 on failure
     */
    int (*cmd)(void *privatedata, unsigned char instr, unsigned int addr,
               unsigned char *buf, unsigned int len);

    /**
     * @brief:  read memory via QSPI (standard read command)
     * @privatedata:  pointer to this Qspi_Device_T
     * @addr:         memory address
     * @buf:          receive buffer
     * @len:          number of bytes to read
     *
     * Return: 0 on success, -1 on failure
     */
    int (*read)(void *privatedata, unsigned int addr, unsigned char *buf,
                unsigned int len);

    /**
     * @brief:  write memory via QSPI (must be pre-erased)
     * @privatedata:  pointer to this Qspi_Device_T
     * @addr:         memory address
     * @buf:          data buffer to write
     * @len:          number of bytes to write
     *
     * Return: 0 on success, -1 on failure
     */
    int (*write)(void *privatedata, unsigned int addr, unsigned char *buf,
                 unsigned int len);

    void *private_data;
} Qspi_Device_T;

/**
 * @brief:  get QSPI device structure by name
 * @name:   device name (e.g. "qspi")
 *
 * Return: pointer to Qspi_Device_T structure, NULL if not found
 */
Qspi_Device_T *get_qspi_device(char *name);

#ifdef __cplusplus
}
#endif

#endif /* _QSPI_DEV_H_ */
