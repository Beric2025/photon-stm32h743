/*
 * Copyright (c) 2026 beric-xiong
 * SPDX-License-Identifier: MIT
 */

#ifndef __BSP_FLASH_H
#define __BSP_FLASH_H

/* FLASH base address */
#define STM32_FLASH_SIZE 0x0200000 /* STM32 FLASH total size */
#define STM32_FLASH_BASE 0x08000000 /* STM32 FLASH base address */
#define FLASH_WAIT_TIME 50000 /* FLASH wait timeout */

/* STM32 H7 FLASH sector base addresses */
/* BANK1 */
#define ADDR_FLASH_SECTOR_0_BANK1 ((unsigned int )0x08000000) /* Sector 0 base address, 128 Kbytes */
#define ADDR_FLASH_SECTOR_1_BANK1 ((unsigned int )0x08020000) /* Sector 1 base address, 128 Kbytes */
#define ADDR_FLASH_SECTOR_2_BANK1 ((unsigned int )0x08040000) /* Sector 2 base address, 128 Kbytes */
#define ADDR_FLASH_SECTOR_3_BANK1 ((unsigned int )0x08060000) /* Sector 3 base address, 128 Kbytes */
#define ADDR_FLASH_SECTOR_4_BANK1 ((unsigned int )0x08080000) /* Sector 4 base address, 128 Kbytes */
#define ADDR_FLASH_SECTOR_5_BANK1 ((unsigned int )0x080A0000) /* Sector 5 base address, 128 Kbytes */
#define ADDR_FLASH_SECTOR_6_BANK1 ((unsigned int )0x080C0000) /* Sector 6 base address, 128 Kbytes */
#define ADDR_FLASH_SECTOR_7_BANK1 ((unsigned int )0x080E0000) /* Sector 7 base address, 128 Kbytes */
#define ADDR_FLASH_SECTOR_END_BANK1 ((unsigned int )0x08100000) /* End address */
#define ADDR_FLASH_SYSTEM_BANK1   ((unsigned int )0x1FF00000) /* System memory base address, 128 Kbytes */
/* BANK2 */
#define ADDR_FLASH_SECTOR_0_BANK2 ((unsigned int )0x08100000) /* Sector 0 base address, 128 Kbytes */
#define ADDR_FLASH_SECTOR_1_BANK2 ((unsigned int )0x08120000) /* Sector 1 base address, 128 Kbytes */
#define ADDR_FLASH_SECTOR_2_BANK2 ((unsigned int )0x08140000) /* Sector 2 base address, 128 Kbytes */
#define ADDR_FLASH_SECTOR_3_BANK2 ((unsigned int )0x08160000) /* Sector 3 base address, 128 Kbytes */
#define ADDR_FLASH_SECTOR_4_BANK2 ((unsigned int )0x08180000) /* Sector 4 base address, 128 Kbytes */
#define ADDR_FLASH_SECTOR_5_BANK2 ((unsigned int )0x081A0000) /* Sector 5 base address, 128 Kbytes */
#define ADDR_FLASH_SECTOR_6_BANK2 ((unsigned int )0x081C0000) /* Sector 6 base address, 128 Kbytes */
#define ADDR_FLASH_SECTOR_7_BANK2 ((unsigned int )0x081E0000) /* Sector 7 base address, 128 Kbytes */
#define ADDR_FLASH_SECTOR_END_BANK2 ((unsigned int )0x08200000) /* End address */
#define ADDR_FLASH_SYSTEM_BANK2   ((unsigned int )0x1FF40000) /* System memory base address, 128 Kbytes */


#define BOARD_NUM_ADDR ADDR_FLASH_SECTOR_1_BANK1

/* Flash partition layout (product-level config) */
#define BOOT_SECTOR_ADDR        ADDR_FLASH_SECTOR_0_BANK1   /* 128 KB */
#define BOOT_SECTOR_SIZE        0x20000
#define SETTING_SECTOR_ADDR     ADDR_FLASH_SECTOR_1_BANK1   /* 128 KB */
#define SETTING_SECTOR_SIZE     0x20000
#define APP_SECTOR_ADDR         ADDR_FLASH_SECTOR_2_BANK1   /* 512 KB */
#define APP_SECTOR_SIZE         0x80000
#define DOWNLOAD_SECTOR_ADDR    ADDR_FLASH_SECTOR_0_BANK2   /* 512 KB */
#define DOWNLOAD_SECTOR_SIZE    0x80000
#define APP_ERASE_SECTORS       APP_SECTOR_SIZE


/**
 * @brief: Get the flash sector number for a given address
 * @addr: flash address
 *
 * Return: 0~7 for Bank1 sectors 0~7, 8~15 for Bank2 sectors 0~7, 0xFF on invalid address
 */
unsigned char stmflash_get_flash_sector(unsigned int addr);

/**
 * @brief: Get the flash sector and bank for a given address
 * @addr: flash address
 * @bank: output parameter, returns the bank (FLASH_BANK_1 or FLASH_BANK_2)
 *
 * Return: 0~7 sector number, 0xFF on invalid address
 */
unsigned char stmflash_get_flash_sector_with_bank(unsigned int addr, unsigned int *bank);

/**
 * @brief: Erase the sector then write data at the specified address
 * @note: STM32H7 sectors are large; sector data cannot be buffered locally.
 *        Writing to a non-0xFF address erases the entire sector without preserving
 *        existing data. Ensure no critical data resides in the sector before writing.
 *        Also works for the OTP region: 0x1FFF7800~0x1FFF7A0F
 * @writeaddr: start address (must be 32-byte aligned)
 * @buff: data buffer pointer
 * @num: number of 32-bit words to write
 *
 * Return: 0-success, 1-invalid address, 2-write error, 3-erase error, 4-timeout
 */
unsigned char stmflash_erase_and_write(unsigned int writeaddr, unsigned int *buff, unsigned int num);

/**
 * @brief: Read a 32-bit word from the specified address
 * @faddr: flash address
 *
 * Return: the 32-bit value read
 */
unsigned int stmflash_read_word(unsigned int faddr);

/**
 * @brief: Erase data of the specified length starting from the given address
 * @writeaddr: start address (must be 4-byte aligned)
 * @num: number of 32-bit words to erase
 *
 * Return: 0-success, 1-invalid address, 2-write error, 3-erase error, 4-timeout
 */
unsigned char stmflash_erase(unsigned int writeaddr, unsigned int num);

/**
 * @brief: Write data of the specified length starting from the given address
 * @note: The sector must be erased beforehand. STM32H7 sectors are large;
 *        sector data cannot be buffered locally.
 * @writeaddr: start address (must be 32-byte aligned)
 * @buff: data buffer pointer
 * @num: number of 32-bit words to write
 *
 * Return: 0-success, 1-invalid address, 2-write error, 3-erase error, 4-timeout
 */
unsigned char stmflash_write(unsigned int writeaddr, unsigned int *buff, unsigned int num);

/**
 * @brief: Read data of the specified length from the given address
 * @raddr: start address
 * @buff: data buffer pointer
 * @length: number of 32-bit words to read
 */
void stmflash_read(unsigned int raddr, unsigned int *buff, unsigned int length);


#endif /*__BSP_FLASH_H*/
