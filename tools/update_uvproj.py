import re

with open('keil/Photon.uvprojx', 'r', encoding='utf-8') as f:
    content = f.read()

# Use forward slashes for matching (XML uses mixed forward/backslash - Windows style backslash)
# For backslash matching in Python strings: '\\' represents one literal backslash
BS = '\\'  # single backslash

# FreeRTOS/LWIP paths
content = content.replace(f'..{BS}FreeRTOS{BS}', f'..{BS}third_party{BS}FreeRTOS{BS}')
content = content.replace('../FreeRTOS/', '../third_party/FreeRTOS/')
content = content.replace(f'..{BS}LWIP{BS}', f'..{BS}third_party{BS}lwIP{BS}')
content = content.replace('../LWIP/', '../third_party/lwIP/')

# BSP files moved to port
content = content.replace(f'..{BS}Interface{BS}GPIO{BS}bsp_gpio.c', f'..{BS}port{BS}stm32h743{BS}bsp{BS}bsp_gpio.c')
content = content.replace(f'..{BS}Interface{BS}UART{BS}bsp_uart.c', f'..{BS}port{BS}stm32h743{BS}bsp{BS}bsp_uart.c')
content = content.replace(f'..{BS}Interface{BS}FDCAN{BS}bsp_fdcan.c', f'..{BS}port{BS}stm32h743{BS}bsp{BS}bsp_fdcan.c')
content = content.replace(f'..{BS}Interface{BS}IIC{BS}bsp_i2c.c', f'..{BS}port{BS}stm32h743{BS}bsp{BS}bsp_i2c.c')
content = content.replace(f'..{BS}Interface{BS}ETHERNET{BS}bsp_eth.c', f'..{BS}port{BS}stm32h743{BS}bsp{BS}bsp_eth.c')
content = content.replace(f'..{BS}Interface{BS}FLASH{BS}stm_flash.c', f'..{BS}port{BS}stm32h743{BS}bsp{BS}stm_flash.c')

# System files moved to port
content = content.replace(f'..{BS}Main{BS}Src{BS}', f'..{BS}port{BS}stm32h743{BS}system{BS}')
content = content.replace(f'..{BS}Main{BS}Inc{BS}', f'..{BS}port{BS}stm32h743{BS}system{BS}')

# Application -> app
content = content.replace(f'..{BS}Application{BS}App{BS}Src{BS}', f'..{BS}app{BS}app_core{BS}Src{BS}')
content = content.replace(f'..{BS}Application{BS}App{BS}Inc{BS}', f'..{BS}app{BS}app_core{BS}Inc{BS}')
content = content.replace(f'..{BS}Application{BS}Share{BS}Src{BS}', f'..{BS}app{BS}share{BS}Src{BS}')
content = content.replace(f'..{BS}Application{BS}Share{BS}Inc{BS}', f'..{BS}app{BS}share{BS}Inc{BS}')

# Interface -> interface (lowercase) - only for *_dev.c files (bsp files already moved)
content = content.replace(f'..{BS}Interface{BS}GPIO{BS}gpio_dev.c', f'..{BS}interface{BS}GPIO{BS}gpio_dev.c')
content = content.replace(f'..{BS}Interface{BS}UART{BS}uart_dev.c', f'..{BS}interface{BS}UART{BS}uart_dev.c')
content = content.replace(f'..{BS}Interface{BS}FDCAN{BS}can_dev.c', f'..{BS}interface{BS}FDCAN{BS}can_dev.c')
content = content.replace(f'..{BS}Interface{BS}FLASH{BS}flash_dev.c', f'..{BS}interface{BS}FLASH{BS}flash_dev.c')
content = content.replace(f'..{BS}Interface{BS}IIC{BS}i2c_dev.c', f'..{BS}interface{BS}IIC{BS}i2c_dev.c')
content = content.replace(f'..{BS}Interface{BS}IIC{BS}my_i2c.c', f'..{BS}interface{BS}IIC{BS}my_i2c.c')
content = content.replace(f'..{BS}Interface{BS}SPI{BS}spi_dev.c', f'..{BS}interface{BS}SPI{BS}spi_dev.c')
content = content.replace(f'..{BS}Interface{BS}ETHERNET{BS}eth_dev.c', f'..{BS}interface{BS}ETHERNET{BS}eth_dev.c')
content = content.replace(f'..{BS}Interface{BS}DELAY{BS}delay_dev.c', f'..{BS}interface{BS}DELAY{BS}delay_dev.c')

# Hardware -> drivers
content = content.replace(f'..{BS}Hardware{BS}BATTERY{BS}', f'..{BS}drivers{BS}battery{BS}')
content = content.replace(f'..{BS}Hardware{BS}LIGHT{BS}', f'..{BS}drivers{BS}light{BS}')
content = content.replace(f'..{BS}Hardware{BS}MOTOR{BS}', f'..{BS}drivers{BS}motor{BS}')
content = content.replace(f'..{BS}Hardware{BS}ULTRASONIC{BS}', f'..{BS}drivers{BS}ultrasonic{BS}')
content = content.replace(f'..{BS}Hardware{BS}IAP{BS}', f'..{BS}drivers{BS}iap{BS}')
content = content.replace(f'..{BS}Hardware{BS}NET{BS}', f'..{BS}drivers{BS}net{BS}')
content = content.replace(f'..{BS}Hardware{BS}ETH_PHY{BS}', f'..{BS}drivers{BS}eth_phy{BS}')
content = content.replace(f'..{BS}Hardware{BS}PCF8574{BS}', f'..{BS}drivers{BS}pcf8574{BS}')

with open('keil/Photon.uvprojx', 'w', encoding='utf-8') as f:
    f.write(content)
print('MDK project file updated successfully')
