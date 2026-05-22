/*
 * Copyright (c) 2026 beric-xiong
 * SPDX-License-Identifier: MIT
 */

#ifndef _ETH_DEV_H_
#define _ETH_DEV_H_

#ifdef __cplusplus
 extern "C" {
#endif

#include <stdint.h>

/* RX callback: called when a frame is received (in task context, not ISR) */
typedef void (*eth_rx_callback_t)(void *user_data, uint8_t *data, uint16_t length);

/* Link state callback: called when link status changes */
typedef void (*eth_link_callback_t)(void *user_data, int link_up, int speed, int duplex);

typedef struct {
    /* ETH controller identifier (e.g. "eth1") */
    char *name;

    /**
     * @brief:  initialize ETH peripheral and PHY
     * @privatedata:  ETH handle (Eth_Device_T *)
     * @mac_addr:     MAC address (6 bytes)
     * Return: 0 on success, -1 on failure
     */
    int (*init)(void *privatedata, uint8_t *mac_addr);

    /**
     * @brief:  deinitialize ETH peripheral
     * @privatedata:  ETH handle (Eth_Device_T *)
     * Return: 0 on success, -1 on failure
     */
    int (*deinit)(void *privatedata);

    /**
     * @brief:  send data via ETH
     * @privatedata: ETH handle (Eth_Device_T *)
     * @data:        data buffer to send
     * @length:      number of bytes to send
     * Return: 0 on success, -1 on failure
     */
    int (*send)(void *privatedata, uint8_t *data, uint16_t length);

    /**
     * @brief:  ETH control (ioctl)
     * @privatedata:  ETH handle (Eth_Device_T *)
     * @cmd:          control command
     * @arg:          command argument
     * Return: 0 on success, -1 on failure
     */
    int (*ioctl)(void *privatedata, unsigned int cmd, void *arg);

    /**
     * @brief:  block waiting for RX frames and process via registered rx_callback
     * @privatedata:  ETH handle (Eth_Device_T *)
     * @timeout_ms:   max wait time in ms (portMAX_DELAY for infinite)
     * Return: number of frames processed, -1 on error
     */
    int (*wait_rx)(void *privatedata, unsigned int timeout_ms);

    /**
     * @brief:  register RX callback (called from wait_rx for each received frame)
     * @privatedata:  ETH handle (Eth_Device_T *)
     * @cb:           callback function
     * @user_data:    user data passed to callback
     * Return: 0 on success, -1 on failure
     */
    int (*register_rx_callback)(void *privatedata, eth_rx_callback_t cb, void *user_data);

    /**
     * @brief:  register link state change callback
     * @privatedata:  ETH handle (Eth_Device_T *)
     * @cb:           callback function
     * @user_data:    user data passed to callback
     * Return: 0 on success, -1 on failure
     */
    int (*register_link_callback)(void *privatedata, eth_link_callback_t cb, void *user_data);

    /**
     * @brief:  start ETH DMA and enable RX/TX
     * @privatedata:  ETH handle (Eth_Device_T *)
     * Return: 0 on success, -1 on failure
     */
    int (*start)(void *privatedata);

    /**
     * @brief:  stop ETH DMA and disable RX/TX
     * @privatedata:  ETH handle (Eth_Device_T *)
     * Return: 0 on success, -1 on failure
     */
    int (*stop)(void *privatedata);

    /**
     * @brief:  read PHY link status
     * @privatedata:  ETH handle (Eth_Device_T *)
     * Return: 1 = link up, 0 = link down, -1 on error
     */
    int (*get_link_status)(void *privatedata);

    void *private_data;
} Eth_Device_T;

/**
 * @brief:  get ETH device structure by name
 * @name:   device name (e.g. "eth1")
 * Return: pointer to Eth_Device_T structure, NULL if not found
 */
Eth_Device_T *get_eth_device(char *name);

#ifdef __cplusplus
}
#endif

#endif /* _ETH_DEV_H_ */
