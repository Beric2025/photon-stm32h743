# Porting Guide

This guide explains how to port this framework to a new MCU platform.

## Architecture Overview

```
app/            Application logic — platform-independent
drivers/        Device drivers — use interface/ abstractions
interface/      Abstract interfaces — pure C structs, no MCU headers
port/<mcu>/     Platform port — MCU-specific BSP implementation
third_party/    Unmodified third-party code
```

## Design Rule

The `interface/` layer defines abstract device operations via structs of function pointers (e.g., `Gpio_Device_T`, `Uart_Device_T`).
The `port/<mcu>/bsp/` layer implements these interfaces for a specific MCU.
Upper layers never call HAL or MCU registers directly.

## Porting Steps

### 1. Create Port Directory

```
port/<your-mcu>/
├── bsp/                  # BSP implementations
│   ├── bsp_gpio.c/h      # GPIO HAL init
│   ├── bsp_uart.c/h      # UART HAL init
│   ├── bsp_i2c.c/h       # I2C HAL init
│   ├── bsp_spi.c/h       # SPI HAL init
│   ├── bsp_fdcan.c/h     # CAN HAL init
│   ├── bsp_eth.c/h       # Ethernet HAL init
│   └── stm_flash.c/h     # Internal flash HAL
├── system/               # System files
│   ├── main.c            # Entry point
│   ├── main.h            # Main header (includes HAL)
│   ├── gpio.c/h          # GPIO IRQ handlers
│   ├── *_hal_conf.h      # HAL configuration
│   ├── *_hal_msp.c       # HAL MSP
│   ├── *_hal_timebase_tim.c  # HAL timebase (SysTick replacement for FreeRTOS)
│   ├── *_it.c/h          # Interrupt handlers
│   ├── syscalls.c        # Retargeted syscalls (printf, etc.)
│   └── system_*.c        # System init (clock, etc.)
├── startup/              # Startup assembly (toolchain-specific subdirectories)
│   ├── startup_*.s       # Common startup (Keil / IAR)
│   └── gcc/              # GCC-specific startup
│       └── startup_*.s
└── <CHIP>_FLASH.ld       # Linker script
```

### 2. Implement BSP Functions

Each BSP file must provide init functions that configure the MCU peripherals.
See `port/stm32h743/bsp/bsp_gpio.c` for an example.

Key pattern:
```c
// bsp_gpio.c - platform-specific GPIO initialization
#include "main.h"  // your MCU HAL

void bsp_gpiob_pin0_init(void)
{
    // Enable clocks, configure pins with your HAL
    __HAL_RCC_GPIOB_CLK_ENABLE();
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = GPIO_PIN_0;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
}
```

### 3. Implement System Files

- `main.c`: Call `HAL_Init()`, clock config, then `application_init()`
- `*_hal_msp.c`: HAL MSP callbacks
- `*_it.c`: Interrupt service routines
- `system_*.c`: System clock configuration

### 4. Configure Build System

**Include paths** — add the following to your build:

- `interface/` and all `interface/<MODULE>/` subdirectories
- `drivers/` and all `drivers/<device>/` subdirectories
- `port/<your-mcu>/bsp/`
- `port/<your-mcu>/system/`
- `app/app_core/Inc/`
- `app/share/Inc/`
- `third_party/FreeRTOS/include/` (if using FreeRTOS)
- `port/freertos/` (FreeRTOSConfig.h location)
- `third_party/lwIP/src/include/` (if using lwIP)
- `port/lwip/arch/` (lwIP adaptation headers)
- Your vendor HAL/CMSIS include paths

**Linker script** — provide `<CHIP>_FLASH.ld` in the port root and reference it via `-T`:

```cmake
set(LINKER_SCRIPT ${CMAKE_SOURCE_DIR}/port/${STM32_MCU}/${STM32_CHIP}_FLASH.ld)
target_link_options(${PROJECT_NAME} PRIVATE -T${LINKER_SCRIPT})
```

**Startup file** — select the correct toolchain variant:
- GCC: `port/<your-mcu>/startup/gcc/startup_*.s`
- Keil/IAR: `port/<your-mcu>/startup/startup_*.s`

**CMake integration** — update `port/CMakeLists.txt` to add your BSP sources and the MCU port directory to header search paths. See the existing `port/CMakeLists.txt` for the pattern.

### 5. Configure RTOS and Network Stack

#### FreeRTOS

Place `FreeRTOSConfig.h` in `port/freertos/`. Key settings to adjust for a new MCU:

- `configCPU_CLOCK_HZ` — match your system clock
- `configTICK_RATE_HZ` — typically 1000 Hz
- `configTOTAL_HEAP_SIZE` — adjust based on available RAM
- `configSYSTICK_CLOCK_HZ_HZ` — must match `SystemCoreClock`

Reference: `port/freertos/FreeRTOSConfig.h`

#### lwIP

The lwIP adaptation layer lives in `port/lwip/arch/`:

| File | Purpose |
|---|---|
| `lwipopts.h` | lwIP compile-time options (memory, protocols, debugging) |
| `cc.h` | Compiler/platform types and macros |
| `sys_arch.c/h` | RTOS adaptation (mutex, semaphore, mailbox, thread) |
| `ethernetif.c/h` | Ethernet interface driver (MAC <-> lwIP glue) |

When porting to a new MCU with a different Ethernet MAC:
1. Update `ethernetif.c` to use the new MAC HAL
2. Verify `sys_arch.c` is compatible with your RTOS (FreeRTOS is the default)
3. Review `lwipopts.h` memory settings against your MCU's RAM

### 6. Implement Interface Functions

The `*_dev.c` files in `interface/` define abstract device operations via structs of function pointers. During initialization, your BSP functions are registered into these structs, wiring the abstract interface to your hardware.

For each interface module your application uses:

1. Read the `*_dev.h` header to understand the expected function signatures
2. Implement the corresponding BSP functions in `port/<your-mcu>/bsp/`
3. Call the registration/init function (e.g., `gpio_dev_init()`) from `main.c` or `application_init()`

Example — GPIO registration flow:
1. `interface/gpio/gpio_dev.h` declares `Gpio_Device_T` with function pointer fields
2. `port/stm32h743/bsp/bsp_gpio.c` implements the hardware-specific init functions
3. `port/stm32h743/system/gpio.c` wires BSP functions into `Gpio_Device_T` structs
4. `port/stm32h743/system/main.c` calls `gpio_dev_init()` at startup

## Reference Platform

See `port/stm32h743/` for a complete working example using:
- STM32H743IITx MCU
- STM32Cube HAL
- FreeRTOS
- lwIP TCP/IP stack

## Adding New Hardware Drivers

1. Create `drivers/<device>/` directory
2. Define device abstraction (`*_dev.h`) — reference `interface/gpio/gpio_dev.h` for the pattern
3. Implement driver (`*_dev.c`) — call hardware exclusively through `interface/` modules
4. Register in `app/` initialization

## Design Checklist

- [ ] `interface/` files contain NO `#include "main.h"` or any HAL header
- [ ] `drivers/` files use only `interface/` abstractions for hardware access
- [ ] `app/` files are platform-independent
- [ ] All MCU-specific code is under `port/<mcu>/`
- [ ] BSP functions are the ONLY place touching HAL/registers
