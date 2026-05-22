# STM32H7xx HAL Driver
file(GLOB HAL_SOURCES CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/*.c
)

add_library(hal STATIC ${HAL_SOURCES})
target_include_directories(hal PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/Inc
    ${CMAKE_CURRENT_SOURCE_DIR}/Inc/Legacy
)
target_link_libraries(hal PUBLIC project_config)
