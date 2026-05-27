/*
 * Copyright (c) 2026 beric-xiong
 * SPDX-License-Identifier: MIT
 */

#ifndef _DELAY_DEV_H_
#define _DELAY_DEV_H_

#ifdef __cplusplus
extern "C" {
#endif


/**
 * @brief: microsecond delay (busy-wait)
 * @us: delay time in microseconds
 */
void delay_us(unsigned int us);

/**
 * @brief: millisecond delay (busy-wait)
 * @ms: delay time in milliseconds
 */
void delay_ms(unsigned int ms);

#ifdef __cplusplus
}
#endif

#endif /* _DELAY_DEV_H_ */
