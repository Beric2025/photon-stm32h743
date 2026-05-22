/*
 * Copyright (c) 2026 beric-xiong
 * SPDX-License-Identifier: MIT
 *
 * stm flash
 */

#include "stm_flash.h"
#include "stm32h7xx_hal.h"


unsigned char stmflash_get_flash_sector(unsigned int addr)
{
    if (addr < ADDR_FLASH_SECTOR_1_BANK1) return 0;
    else if (addr < ADDR_FLASH_SECTOR_2_BANK1) return 1;
    else if (addr < ADDR_FLASH_SECTOR_3_BANK1) return 2;
    else if (addr < ADDR_FLASH_SECTOR_4_BANK1) return 3;
    else if (addr < ADDR_FLASH_SECTOR_5_BANK1) return 4;
    else if (addr < ADDR_FLASH_SECTOR_6_BANK1) return 5;
    else if (addr < ADDR_FLASH_SECTOR_7_BANK1) return 6;
    else if (addr < ADDR_FLASH_SECTOR_END_BANK1) return 7;
    else if (addr < ADDR_FLASH_SECTOR_1_BANK2) return 8;
    else if (addr < ADDR_FLASH_SECTOR_2_BANK2) return 9;
    else if (addr < ADDR_FLASH_SECTOR_3_BANK2) return 10;
    else if (addr < ADDR_FLASH_SECTOR_4_BANK2) return 11;
    else if (addr < ADDR_FLASH_SECTOR_5_BANK2) return 12;
    else if (addr < ADDR_FLASH_SECTOR_6_BANK2) return 13;
    else if (addr < ADDR_FLASH_SECTOR_7_BANK2) return 14;
    else if (addr < ADDR_FLASH_SECTOR_END_BANK2) return 15;
    return 0xFF;
}

unsigned char stmflash_get_flash_sector_with_bank(unsigned int addr, unsigned int *bank)
{
    unsigned char sector = 0;

    if (addr < ADDR_FLASH_SECTOR_1_BANK1) {
        sector = 0;
        *bank = FLASH_BANK_1;
    }
    else if (addr < ADDR_FLASH_SECTOR_2_BANK1) {
        sector = 1;
        *bank = FLASH_BANK_1;
    }
    else if (addr < ADDR_FLASH_SECTOR_3_BANK1) {
        sector = 2;
        *bank = FLASH_BANK_1;
    }
    else if (addr < ADDR_FLASH_SECTOR_4_BANK1) {
        sector = 3;
        *bank = FLASH_BANK_1;
    }
    else if (addr < ADDR_FLASH_SECTOR_5_BANK1) {
        sector = 4;
        *bank = FLASH_BANK_1;
    }
    else if (addr < ADDR_FLASH_SECTOR_6_BANK1) {
        sector = 5;
        *bank = FLASH_BANK_1;
    }
    else if (addr < ADDR_FLASH_SECTOR_7_BANK1) {
        sector = 6;
        *bank = FLASH_BANK_1;
    }
    else if (addr < ADDR_FLASH_SECTOR_END_BANK1) {
        sector = 7;
        *bank = FLASH_BANK_1;
    }
    else if (addr < ADDR_FLASH_SECTOR_1_BANK2) {
        sector = 0;
        *bank = FLASH_BANK_2;
    }
    else if (addr < ADDR_FLASH_SECTOR_2_BANK2) {
        sector = 1;
        *bank = FLASH_BANK_2;
    }
    else if (addr < ADDR_FLASH_SECTOR_3_BANK2) {
        sector = 2;
        *bank = FLASH_BANK_2;
    }
    else if (addr < ADDR_FLASH_SECTOR_4_BANK2) {
        sector = 3;
        *bank = FLASH_BANK_2;
    }
    else if (addr < ADDR_FLASH_SECTOR_5_BANK2) {
        sector = 4;
        *bank = FLASH_BANK_2;
    }
    else if (addr < ADDR_FLASH_SECTOR_6_BANK2) {
        sector = 5;
        *bank = FLASH_BANK_2;
    }
    else if (addr < ADDR_FLASH_SECTOR_7_BANK2) {
        sector = 6;
        *bank = FLASH_BANK_2;
    }
    else if (addr < ADDR_FLASH_SECTOR_END_BANK2) {
        sector = 7;
        *bank = FLASH_BANK_2;
    }
    else {
        sector = 0xFF;
        *bank = 0;
    }

    return sector;
}

unsigned char stmflash_erase_and_write(unsigned int writeaddr, unsigned int *buff, unsigned int num)
{
    unsigned char ret = 0;
    FLASH_EraseInitTypeDef flash_erase_init;
    HAL_StatusTypeDef flash_status = HAL_OK;
    uint32_t sectorError = 0;
    unsigned int add_rx = 0;
    unsigned int end_addr = 0;
    unsigned int bank = 0;
    unsigned char sector_num = 0;

    if (writeaddr < STM32_FLASH_BASE || writeaddr % 32 ||
        writeaddr >= (STM32_FLASH_BASE + STM32_FLASH_SIZE))   {
        ret = 1;
        return ret;
    }

    HAL_FLASH_Unlock();
    add_rx = writeaddr;
    end_addr = writeaddr + num * 4;
    if (add_rx < ADDR_FLASH_SYSTEM_BANK1) {
        while (add_rx < end_addr) {
            if(stmflash_read_word(add_rx) != 0XFFFFFFFF) {
                sector_num = stmflash_get_flash_sector_with_bank(add_rx, &bank);
                if(sector_num == 0xFF) {
                    ret = 1;
                    break;
                }

                flash_status = FLASH_WaitForLastOperation(FLASH_WAIT_TIME, bank);
                if (flash_status != HAL_OK) {
                    ret = 4;
                    break;
                }

                flash_erase_init.Banks = bank;
                flash_erase_init.TypeErase = FLASH_TYPEERASE_SECTORS;
                flash_erase_init.Sector = sector_num;
                flash_erase_init.NbSectors = 1;
                flash_erase_init.VoltageRange = FLASH_VOLTAGE_RANGE_3;

                if (HAL_FLASHEx_Erase(&flash_erase_init, &sectorError) != HAL_OK) {
                    ret = 3;
                    break;
                }
                SCB_CleanInvalidateDCache();
            }
            else  {
                add_rx += 4;
            }
        }

        if (ret == 0) {
            while (writeaddr < end_addr) {
                sector_num = stmflash_get_flash_sector_with_bank(writeaddr, &bank);
                if(sector_num == 0xFF) {
                    ret = 1;
                    break;
                }

                flash_status = FLASH_WaitForLastOperation(FLASH_WAIT_TIME, bank);
                if (flash_status != HAL_OK) {
                    ret = 4;
                    break;
                }
                if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_FLASHWORD, writeaddr, (unsigned int)buff) != HAL_OK) {
                    ret = 2;
                    break;
                }
                writeaddr += 32;
                buff+=8;
            }
        }
    }

    HAL_FLASH_Lock();
    return ret;
}


unsigned int stmflash_read_word(unsigned int faddr)
{
    return *(__IO unsigned int *)faddr;
}

unsigned char stmflash_erase(unsigned int writeaddr, unsigned int num)
{
    unsigned char ret = 0;
    HAL_StatusTypeDef flash_status = HAL_OK;
    FLASH_EraseInitTypeDef flash_erase_init;
    uint32_t sectorError = 0;
    unsigned int add_rx = 0;
    unsigned int end_addr = 0;
    unsigned int bank = 0;
    unsigned char sector_num = 0;

    if (writeaddr < STM32_FLASH_BASE || writeaddr % 32 ||
        writeaddr >= (STM32_FLASH_BASE + STM32_FLASH_SIZE))   {
        ret = 1;
        return ret;
    }

    HAL_FLASH_Unlock();
    add_rx = writeaddr;
    end_addr = writeaddr + num * 4;

    if (add_rx < ADDR_FLASH_SYSTEM_BANK1) {
        while (add_rx < end_addr) {
            if(stmflash_read_word(add_rx)!=0XFFFFFFFF) {
                sector_num = stmflash_get_flash_sector_with_bank(add_rx, &bank);
                if(sector_num == 0xFF) {
                    ret = 1;
                    break;
                }

                flash_status = FLASH_WaitForLastOperation(FLASH_WAIT_TIME, bank);
                if (flash_status != HAL_OK) {
                    ret = 4;
                    break;
                }

                flash_erase_init.Banks = bank;
                flash_erase_init.TypeErase = FLASH_TYPEERASE_SECTORS;
                flash_erase_init.Sector = sector_num;
                flash_erase_init.NbSectors = 1;
                flash_erase_init.VoltageRange = FLASH_VOLTAGE_RANGE_3;

                if (HAL_FLASHEx_Erase(&flash_erase_init, &sectorError) != HAL_OK) {
                    ret = 3;
                    break;
                }
                SCB_CleanInvalidateDCache();
            }
            else  {
                add_rx += 4;
            }
        }
    }

    HAL_FLASH_Lock();
    return ret;
}

unsigned char stmflash_write(unsigned int writeaddr, unsigned int *buff, unsigned int num)
{
    unsigned char ret = 0;
    HAL_StatusTypeDef flash_status = HAL_OK;
    unsigned int end_addr = 0;
    unsigned int bank = 0;
    unsigned char sector_num = 0;

    if (writeaddr < STM32_FLASH_BASE || writeaddr % 32 ||
        writeaddr >= (STM32_FLASH_BASE + STM32_FLASH_SIZE))   {
        ret = 1;
        return ret;
    }

    HAL_FLASH_Unlock();
    end_addr = writeaddr + num * 4;

    while (writeaddr < end_addr) {
        sector_num = stmflash_get_flash_sector_with_bank(writeaddr, &bank);
        if(sector_num == 0xFF) {
            ret = 1;
            break;
        }

        flash_status = FLASH_WaitForLastOperation(FLASH_WAIT_TIME, bank);
        if (flash_status != HAL_OK) {
            ret = 4;
            break;
        }

        if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_FLASHWORD, writeaddr, (unsigned int)buff) != HAL_OK) {
            ret = 2;
            break;
        }
        writeaddr += 32;
        buff+=8;
    }
    HAL_FLASH_Lock();
    return ret;
}


void stmflash_read(unsigned int raddr, unsigned int *buff, unsigned int length)
{
    unsigned int i;
    for (i = 0; i < length; i++) {
        buff [i] = stmflash_read_word(raddr);
        raddr+= 4;
    }
}
