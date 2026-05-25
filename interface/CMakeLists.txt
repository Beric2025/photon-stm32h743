# Interface abstraction layer — wraps HAL peripherals with RTOS-safe APIs
file(GLOB_RECURSE IFACE_SOURCES CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/*.c
)
add_library(interface STATIC ${IFACE_SOURCES})
target_include_directories(interface PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/GPIO
    ${CMAKE_CURRENT_SOURCE_DIR}/UART
    ${CMAKE_CURRENT_SOURCE_DIR}/FDCAN
    ${CMAKE_CURRENT_SOURCE_DIR}/FLASH
    ${CMAKE_CURRENT_SOURCE_DIR}/IIC
    ${CMAKE_CURRENT_SOURCE_DIR}/SPI
    ${CMAKE_CURRENT_SOURCE_DIR}/ETHERNET
    ${CMAKE_CURRENT_SOURCE_DIR}/DELAY
    ${CMAKE_CURRENT_SOURCE_DIR}/ADC
    # TODO: cross-module references — these belong in the modules that own them
    ${CMAKE_SOURCE_DIR}/drivers/pcf8574
    ${CMAKE_SOURCE_DIR}/drivers/eth_phy
    ${CMAKE_SOURCE_DIR}/app/share/Inc
)
target_link_libraries(interface PUBLIC freertos_kernel hal)
