# Device drivers — depends on interface, FreeRTOS, HAL
file(GLOB_RECURSE DRIVER_SOURCES CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/*.c
)
add_library(drivers STATIC ${DRIVER_SOURCES})
target_include_directories(drivers PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/battery
    ${CMAKE_CURRENT_SOURCE_DIR}/light
    ${CMAKE_CURRENT_SOURCE_DIR}/motor
    ${CMAKE_CURRENT_SOURCE_DIR}/ultrasonic
    ${CMAKE_CURRENT_SOURCE_DIR}/iap
    ${CMAKE_CURRENT_SOURCE_DIR}/eth_phy
    ${CMAKE_CURRENT_SOURCE_DIR}/net
    ${CMAKE_CURRENT_SOURCE_DIR}/pcf8574
    # TODO: cross-module references — these should come from their owning targets
    ${CMAKE_SOURCE_DIR}/app/app_core/Inc
    ${CMAKE_SOURCE_DIR}/app/share/Inc
)
target_link_libraries(drivers PUBLIC freertos_kernel interface hal lwip_config)
