# Platform port — BSP + lwIP port adaptation
file(GLOB_RECURSE PORT_SOURCES CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/${STM32_MCU}/bsp/*.c
    ${CMAKE_CURRENT_SOURCE_DIR}/lwip/arch/*.c
    ${CMAKE_CURRENT_SOURCE_DIR}/lwip/common/*.c
)

add_library(port STATIC ${PORT_SOURCES})
target_include_directories(port PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/${STM32_MCU}/bsp
    ${CMAKE_CURRENT_SOURCE_DIR}/${STM32_MCU}/system
    ${CMAKE_CURRENT_SOURCE_DIR}/freertos
    ${CMAKE_CURRENT_SOURCE_DIR}/lwip/arch
    ${CMAKE_CURRENT_SOURCE_DIR}/lwip/common
)
# TODO: port -> drivers is an inverted dependency; BSP should not depend on device drivers
target_link_libraries(port PRIVATE drivers lwipcore)
