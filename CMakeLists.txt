cmake_minimum_required(VERSION 3.16)
project(stm32-embedded-framework C CXX ASM)

# ============================================================
# 1. Version from git
# ============================================================
execute_process(
    COMMAND git describe --tags --always --dirty
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    OUTPUT_VARIABLE GIT_VERSION
    OUTPUT_STRIP_TRAILING_WHITESPACE
    ERROR_QUIET
)
if(NOT GIT_VERSION)
    set(GIT_VERSION "v0.0.0-dev")
endif()
message(STATUS "Version: ${GIT_VERSION}")

# ============================================================
# 2. MCU configuration (override via -D on command line)
# ============================================================
set(STM32_MCU    "stm32h743"     CACHE STRING "Target MCU family (stm32h743, stm32h750, etc.)")
set(STM32_DEVICE "STM32H743xx"   CACHE STRING "MCU device define (e.g., STM32H743xx)")
set(STM32_CHIP   "STM32H743IITx" CACHE STRING "MCU chip model for linker script selection")
set(USE_PWR_LDO_SUPPLY ON CACHE BOOL "Use LDO power supply (disable for SMPS)")
option(BUILD_DRIVERS "Build drivers and application layers" ON)

# Derive CMSIS device family from MCU name prefix: stm32h743 -> STM32H7xx
string(SUBSTRING "${STM32_MCU}" 5 1 _MCU_LETTER)
string(SUBSTRING "${STM32_MCU}" 6 1 _MCU_SERIES)
string(TOUPPER "${_MCU_LETTER}" _MCU_LETTER_UPPER)
set(CMSIS_DEVICE_FAMILY "STM32${_MCU_LETTER_UPPER}${_MCU_SERIES}xx")

# ============================================================
# 3. project_config — hardware definitions and CMSIS headers
#    Only targets that link this inherit MCU settings.
# ============================================================
add_library(project_config INTERFACE)
target_compile_definitions(project_config INTERFACE
    ${STM32_DEVICE}
    USE_HAL_DRIVER
    $<$<BOOL:${USE_PWR_LDO_SUPPLY}>:USE_PWR_LDO_SUPPLY>
    APP_VERSION="${GIT_VERSION}"
    BUILD_TYPE="${CMAKE_BUILD_TYPE}"
    $<$<CONFIG:Debug>:DEBUG_BUILD>
)
target_include_directories(project_config INTERFACE
    third_party/CMSIS/Include
    third_party/CMSIS/Device/ST/${CMSIS_DEVICE_FAMILY}/Include
    port/${STM32_MCU}/system
    port/${STM32_MCU}/bsp
)

# ============================================================
# 4. lwip_config — lwIP options and port adaptation headers
# ============================================================
add_library(lwip_config INTERFACE)
target_include_directories(lwip_config INTERFACE
    port/lwip
    port/lwip/arch
    port/lwip/common
    third_party/lwIP/src/include
    third_party/lwIP
)
target_link_libraries(lwip_config INTERFACE project_config)

# ============================================================
# 5. freertos_config — FreeRTOSConfig.h location and port selection
# ============================================================
set(FREERTOS_PORT GCC_ARM_CM7 CACHE STRING "FreeRTOS port")
set(FREERTOS_HEAP 4 CACHE STRING "FreeRTOS heap implementation")

add_library(freertos_config INTERFACE)
target_include_directories(freertos_config INTERFACE port/freertos)
target_link_libraries(freertos_config INTERFACE project_config)

# ============================================================
# 6. C standard
# ============================================================
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# ============================================================
# 7. Subdirectories
# ============================================================
add_subdirectory(third_party/FreeRTOS)
add_subdirectory(third_party/STM32H7xx_HAL_Driver)
add_subdirectory(third_party/lwIP)
# Wire lwip_config into lwipcore (created by lwIP's Filelists.cmake)
target_link_libraries(lwipcore PUBLIC lwip_config)

add_subdirectory(interface)
if(BUILD_DRIVERS)
    add_subdirectory(drivers)
    add_subdirectory(app)
endif()
add_subdirectory(port)

# ============================================================
# 8. Executable
# ============================================================
set(PORT_DIR port/${STM32_MCU})
set(LINKER_SCRIPT ${CMAKE_SOURCE_DIR}/${PORT_DIR}/${STM32_CHIP}_FLASH.ld)

file(GLOB EXE_SOURCES CONFIGURE_DEPENDS
    ${PORT_DIR}/system/*.c
    ${PORT_DIR}/startup/gcc/*.s
)
add_executable(${PROJECT_NAME} ${EXE_SOURCES})
set_target_properties(${PROJECT_NAME} PROPERTIES SUFFIX ".elf")

# --start-group required: port <-> lwipcore form a circular dependency
# (lwipcore calls sys_arch_* from port/lwip/arch/sys_arch.c,
#  port/lwip/arch/ethernetif.c calls lwIP stack APIs from lwipcore).
# This is inherent to lwIP's OS-port architecture — not a build bug.
set(EXE_LINK_LIBS interface port freertos_kernel hal lwipcore)
if(BUILD_DRIVERS)
    list(APPEND EXE_LINK_LIBS drivers app)
endif()

target_link_libraries(${PROJECT_NAME} PRIVATE
    project_config
    -Wl,--start-group
    ${EXE_LINK_LIBS}
    -Wl,--end-group
)

target_link_options(${PROJECT_NAME} PRIVATE
    -T${LINKER_SCRIPT}
    -Wl,-Map=${PROJECT_NAME}.map
    --specs=nano.specs
    --specs=nosys.specs
)

# ============================================================
# 9. Post-build: .bin .hex .dis and size
# ============================================================
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_OBJCOPY} -O binary $<TARGET_FILE:${PROJECT_NAME}> ${PROJECT_NAME}.bin
    COMMAND ${CMAKE_OBJCOPY} -Oihex $<TARGET_FILE:${PROJECT_NAME}> ${PROJECT_NAME}.hex
    COMMAND ${CMAKE_OBJDUMP} -d $<TARGET_FILE:${PROJECT_NAME}> > ${PROJECT_NAME}.dis
    COMMAND ${CMAKE_SIZE} --format=berkeley $<TARGET_FILE:${PROJECT_NAME}>
    COMMENT "Generating .bin, .hex, .dis and printing size..."
)

# ============================================================
# 10. Dirty working tree warning
# ============================================================
if(GIT_VERSION MATCHES ".*-dirty$")
    message(WARNING "Building from dirty working tree — not for release")
endif()
