/*
 * Copyright (c) 2026 beric-xiong
 * SPDX-License-Identifier: MIT
 */

#ifndef _ADC_DEV_H_
#define _ADC_DEV_H_

#ifdef __cplusplus
 extern "C" {
#endif


typedef struct {
    /* ADC device identifier (e.g. "adc1") */
    char *name;

    /**
     * @brief:  initialize ADC
     * @privatedata:  ADC handle (Adc_Device_T *)
     *
     * Return: 0 on success, -1 on failure
     */
    int (*init)(void *privatedata);

    /**
     * @brief:  start ADC conversion
     * @privatedata:  ADC handle (Adc_Device_T *)
     *
     * Return: 0 on success, -1 on failure
     */
    int (*start)(void *privatedata);

    /**
     * @brief:  stop ADC conversion
     * @privatedata:  ADC handle (Adc_Device_T *)
     *
     * Return: 0 on success, -1 on failure
     */
    int (*stop)(void *privatedata);

    /**
     * @brief:  get ADC raw buff for a channel
     * @privatedata:  ADC handle (Adc_Device_T *)
     * @num:          channel index
     * @buff:        buffer for raw ADC buff
     *
     * Return: raw ADC buff (averaged from ring buffer), 0 on failure
     */
    int (*get_raw_buff)(void *privatedata, unsigned char num, unsigned short *buff);

    void *private_data;
} Adc_Device_T;

/**
 * @brief:  get ADC device structure by name
 * @name:   device name (e.g. "adc1")
 *
 * Return: pointer to Adc_Device_T structure, NULL if not found
 */
Adc_Device_T *get_adc_device(char *name);

#ifdef __cplusplus
}
#endif

#endif  /* _ADC_DEV_H_ */
