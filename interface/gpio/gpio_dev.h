/*
 * Copyright (c) 2026 beric-xiong
 * SPDX-License-Identifier: MIT
 */

#ifndef _GPIO_DEV_H_
#define _GPIO_DEV_H_

#ifdef __cplusplus
 extern "C" {
#endif


typedef struct {
	/* GPIO pin identifier (e.g. "gpio1", "gpio2") */
	char *name;

	/**
	 * @brief:  initialize GPIO pin
	 * @private_data:  GPIO handle (Gpio_Device_T *)
	 *
	 * Return: 0 on success, -1 on failure
	 */
	int (*init)(void *privatedata);

	/**
	 * @brief:  write GPIO pin output level
	 * @private_data:  GPIO handle (Gpio_Device_T *)
	 * @value:         output value (0 for low, non-zero for high)
	 *
	 * Return: 0 on success, -1 on failure
	 */
	int (*write_pin)(void *privatedata, unsigned char value);

	/**
	 * @brief:  read GPIO pin input level
	 * @private_data:  GPIO handle (Gpio_Device_T *)
	 *
	 * Return: pin level (0 for low, 1 for high)
	 */
	unsigned char (*read_pin)(void *privatedata);

	/**
	 * @brief:  toggle GPIO pin output level
	 * @private_data:  GPIO handle (Gpio_Device_T *)
	 *
	 * Return: 0 on success, -1 on failure
	 */
	int (*toggle_pin)(void *privatedata);

	/**
	 * @brief:  GPIO pin control (ioctl)
	 * @private_data:  GPIO handle (Gpio_Device_T *)
	 * @flag:          control command
	 *
	 * Return: 0 on success, -1 on failure
	 */
	int (*ioctl)(void *privatedata, unsigned int flag);

	/**
	 * @brief:  get GPIO interrupt flag
	 * @privatedata:  GPIO handle (GpioDev_T *)
	 * @value:        buffer for interrupt flag (0 = no interrupt, 1 = active)
	 *
	 * Return: 0 on success, -1 on failure
	 */
	int (*get_interruput)(void *privatedata, unsigned char *value);

	/**
	 * @brief:  clear GPIO interrupt flag
	 * @privatedata:  GPIO handle (GpioDev_T *)
	 *
	 * Return: 0 on success, -1 on failure
	 */
	int (*clear_interruput)(void *privatedata);

	void *private_data;
} Gpio_Device_T;

/**
 * @brief:  get GPIO device structure by name
 * @name:   device name (e.g. "led0", "led1")
 *
 * Return: pointer to Gpio_Device_T structure, NULL if not found
 */
Gpio_Device_T *get_gpio_device(char *name);

#ifdef __cplusplus
}
#endif

#endif	/* _GPIO_DEV_H_ */
