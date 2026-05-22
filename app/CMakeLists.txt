# Application layer — depends on drivers and interface
file(GLOB_RECURSE APP_SOURCES CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/app_core/Src/*.c
    ${CMAKE_CURRENT_SOURCE_DIR}/share/Src/*.c
)
add_library(app STATIC ${APP_SOURCES})
target_include_directories(app PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/app_core/Inc
    ${CMAKE_CURRENT_SOURCE_DIR}/share/Inc
)
target_link_libraries(app PUBLIC drivers interface freertos_kernel)
