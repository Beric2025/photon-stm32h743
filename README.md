# STM32 Embedded Framework

A layered, portable embedded firmware framework designed for clean separation
between MCU-specific code and hardware-independent device drivers.

## Architecture

```
┌─────────────────────────┐
│  app/                   │  Application logic (no hardware dependency)
├─────────────────────────┤
│  drivers/               │  Device drivers (pure protocol, HW via interface)
├─────────────────────────┤
│  interface/             │  Abstract interfaces (pure C structs, zero MCU code)
├─────────────────────────┤
│  port/<mcu>/            │  Platform port — MCU-specific BSP implementation
├─────────────────────────┤
│  third_party/           │  Third-party libraries (FreeRTOS, lwIP, CMSIS)
└─────────────────────────┘
```

## Directory Layout

| Directory | Purpose |
|---|---|
| `app/` | Application logic: task management, LED control, networking, versioning |
| `drivers/` | External peripheral drivers: battery, motor, light, ultrasonic, IAP, etc. |
| `interface/` | Hardware abstraction interfaces: GPIO, UART, I2C, SPI, CAN, ETH, FLASH, DELAY |
| `port/stm32h743/` | STM32H743 BSP port (example platform): HAL init, startup, system config |
| `port/freertos/` | FreeRTOS configuration (`FreeRTOSConfig.h`, heap selection) |
| `port/lwip/` | lwIP arch adaptation (`sys_arch.c`, `ethernetif.c`, `lwipopts.h`) |
| `third_party/` | Third-party libraries: FreeRTOS (git submodule), lwIP, CMSIS, HAL |
| `tools/` | Auxiliary scripts (`update_uvproj.py` — sync Keil project files) |

## Design Rules

1. **interface/** defines only abstract types — no HAL, no RTOS, no MCU headers.
2. **drivers/** communicates with hardware exclusively through `interface/` structs.
3. **port/** implements the `interface/` contracts for a specific MCU.
4. **app/** orchestrates drivers; no direct hardware access.

## Porting to a New MCU

1. Create `port/<your-mcu>/bsp/` with BSP implementations matching `interface/` contracts.
2. Provide `port/<your-mcu>/system/` with startup, clock config, and `main.c`.
3. Update build system include paths to point to your new `port/<your-mcu>/` directories.

See [docs/porting_guide.md](docs/porting_guide.md) for detailed instructions.

## Platform Example: STM32H743

The `port/stm32h743/` directory contains a complete working example for the
STM32H743IITx MCU using STM32Cube HAL and FreeRTOS. Use it as a reference
when porting to other MCUs.

## Build

### Clone (with submodules)

```bash
git clone --recurse-submodules <repo-url>
# Or if already cloned:
git submodule update --init --recursive
```

### Quick Start (recommended)

```bash
# Linux / macOS / WSL
./cmake/build.sh              # Release build
./cmake/build.sh debug        # Debug build
./cmake/build.sh clean        # Clean

# Windows (cmd)
cmake\build.bat               # Release build
cmake\build.bat debug         # Debug build
cmake\build.bat clean         # Clean

# Or use Make directly
make                    # Release build
make debug              # Debug build
make clean              # Clean
make flash              # Build and flash via openocd (stlink + stm32h7x)
```

### CMake Presets (VS Code / IDE)

```bash
cmake --preset debug          # STM32H743 Debug
cmake --preset release        # STM32H743 Release
cmake --preset h750-debug     # STM32H750 Debug
```

See [`CMakePresets.json`](CMakePresets.json) for all preset definitions.

### Manual CMake

Requires: `arm-none-eabi-gcc`, `cmake` (>= 3.16)

```bash
mkdir build && cd build
cmake .. -DCMAKE_TOOLCHAIN_FILE=cmake/arm-none-eabi-gcc.cmake -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)
```

### Keil (for Windows debugging)

The Keil project file is kept for compatibility:
`keil/Photon.uvprojx`

## Build Configurations

| Preset | Optimization | Macro Defined | Purpose |
|--------|-------------|---------------|---------|
| `debug` | `-Og` | `DEBUG_BUILD` | Development: assertions, verbose logging, debug symbols |
| `release` | `-Os` | *(none)* | Production: size-optimized, debug code stripped |

### Version Injection

The build system automatically injects the git version as a compile-time macro:

| Git state | `APP_VERSION` value |
|-----------|---------------------|
| On tag `v1.0.0` | `v1.0.0` |
| 3 commits after tag | `v1.0.0-3-gabc1234` |
| Uncommitted changes | `v1.0.0-3-gabc1234-dirty` |

### Using in Code

```c
#ifdef DEBUG_BUILD
    #define LOG_LEVEL LOG_LEVEL_DEBUG
#else
    #define LOG_LEVEL LOG_LEVEL_WARN
#endif

// Runtime version string
printf("Firmware: %s (%s)\n", APP_VERSION, BUILD_TYPE);
```

## License

MIT License — see [LICENSE](LICENSE) for details.
