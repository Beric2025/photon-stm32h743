# Toolchain file for arm-none-eabi-gcc
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR arm)

# Locate toolchain binaries — fail early with a clear message if missing
find_program(CMAKE_C_COMPILER   arm-none-eabi-gcc   REQUIRED)
find_program(CMAKE_CXX_COMPILER arm-none-eabi-g++   REQUIRED)
find_program(CMAKE_ASM_COMPILER arm-none-eabi-gcc   REQUIRED)
find_program(CMAKE_AR           arm-none-eabi-ar     REQUIRED)
find_program(CMAKE_OBJCOPY      arm-none-eabi-objcopy REQUIRED)
find_program(CMAKE_OBJDUMP      arm-none-eabi-objdump REQUIRED)
find_program(CMAKE_SIZE         arm-none-eabi-size   REQUIRED)

set(CMAKE_CROSSCOMPILING TRUE)
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

# MCU architecture flags — override via -DMCU_FLAGS on command line
# e.g. -DMCU_FLAGS="-mcpu=cortex-m4 -mthumb -mfloat-abi=hard"
set(MCU_FLAGS "-mcpu=cortex-m7 -mthumb -mfpu=fpv5-d16 -mfloat-abi=hard"
    CACHE STRING "GCC flags for target MCU core/FPU")

set(CMAKE_C_FLAGS   "${MCU_FLAGS} -Wall -Wextra -fdata-sections -ffunction-sections" CACHE INTERNAL "")
set(CMAKE_CXX_FLAGS "${MCU_FLAGS} -Wall -Wextra -fdata-sections -ffunction-sections -fno-rtti -fno-exceptions" CACHE INTERNAL "")
set(CMAKE_ASM_FLAGS "${MCU_FLAGS} -x assembler-with-cpp" CACHE INTERNAL "")
# --specs and -Map belong at the project level (target_link_options), not here
set(CMAKE_EXE_LINKER_FLAGS "${MCU_FLAGS} -Wl,--gc-sections" CACHE INTERNAL "")
