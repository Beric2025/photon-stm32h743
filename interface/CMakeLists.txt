# Interface abstraction layer — wraps HAL peripherals with RTOS-safe APIs
file(GLOB_RECURSE IFACE_SOURCES CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/*.c
)
add_library(interface STATIC ${IFACE_SOURCES})
target_include_directories(interface PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/gpio
    ${CMAKE_CURRENT_SOURCE_DIR}/uart
    ${CMAKE_CURRENT_SOURCE_DIR}/can
    ${CMAKE_CURRENT_SOURCE_DIR}/flash
    ${CMAKE_CURRENT_SOURCE_DIR}/iic
    ${CMAKE_CURRENT_SOURCE_DIR}/spi
    ${CMAKE_CURRENT_SOURCE_DIR}/ethernet
    ${CMAKE_CURRENT_SOURCE_DIR}/adc
    ${CMAKE_CURRENT_SOURCE_DIR}/tim
    # TODO: cross-module references — these belong in the modules that own them
    ${CMAKE_SOURCE_DIR}/drivers/delay
    ${CMAKE_SOURCE_DIR}/drivers/pcf8574
    ${CMAKE_SOURCE_DIR}/drivers/eth_phy
    ${CMAKE_SOURCE_DIR}/app/share/Inc
)
target_link_libraries(interface PUBLIC freertos_kernel hal)
