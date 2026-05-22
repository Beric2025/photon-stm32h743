#!/usr/bin/env bash
#
# build.sh - CMake build script for STM32 Embedded Framework
#
# Usage:
#   ./build.sh              # Release build (default)
#   ./build.sh debug        # Debug build
#   ./build.sh h750-debug   # STM32H750 Debug build
#   ./build.sh clean        # Clean all build directories
#   ./build.sh flash        # Build and flash (via openocd)
#
# Requirements: arm-none-eabi-gcc, cmake (>= 3.16)

set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
PROJECT_DIR="$(cd "${SCRIPT_DIR}/.." && pwd)"
BUILD_TYPE="${1:-release}"

# Colors
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m'

check_deps() {
    local missing=0
    for cmd in arm-none-eabi-gcc cmake make; do
        if ! command -v "$cmd" &>/dev/null; then
            echo -e "${RED}Error: '$cmd' not found in PATH${NC}"
            missing=1
        fi
    done
    if [ "$missing" -ne 0 ]; then
        echo -e "${YELLOW}Install: apt-get install gcc-arm-none-eabi cmake make${NC}"
        exit 1
    fi
}

do_clean() {
    echo -e "${YELLOW}Cleaning all build directories...${NC}"
    rm -rf "${PROJECT_DIR}/build"
    echo -e "${GREEN}Clean complete.${NC}"
}

do_build() {
    check_deps

    local build_type="$1"
    local preset
    local build_dir

    case "${build_type,,}" in
        debug)
            preset="debug"
            build_dir="${PROJECT_DIR}/build/debug"
            ;;
        release)
            preset="release"
            build_dir="${PROJECT_DIR}/build/release"
            ;;
        h750-debug)
            preset="h750-debug"
            build_dir="${PROJECT_DIR}/build/h750-debug"
            ;;
        *)
            echo -e "${RED}Unknown build type: $build_type${NC}"
            echo "Usage: $0 [release|debug|h750-debug|clean|flash]"
            exit 1
            ;;
    esac

    echo -e "${GREEN}=== STM32 Embedded Framework Build ===${NC}"
    echo "Preset: ${preset}"

    echo -e "${YELLOW}Configuring...${NC}"
    cmake --preset "${preset}" -S "${PROJECT_DIR}"

    echo -e "${YELLOW}Building...${NC}"
    cmake --build "${build_dir}" -j"$(nproc 2>/dev/null || echo 4)"

    echo -e "${GREEN}Build successful!${NC}"
    echo "Output: ${build_dir}/stm32-embedded-framework.elf"
}

do_flash() {
    local build_type="${BUILD_TYPE:-release}"
    local preset
    case "${build_type,,}" in
        debug)   preset="debug"   ;;
        release) preset="release" ;;
        *)       preset="release" ;;
    esac
    local build_dir="${PROJECT_DIR}/build/${preset}"
    local elf="${build_dir}/stm32-embedded-framework.elf"

    if [ ! -f "$elf" ]; then
        echo -e "${YELLOW}No ELF found, building first...${NC}"
        do_build "$build_type"
    fi
    echo -e "${YELLOW}Flashing via OpenOCD...${NC}"
    openocd -f interface/stlink.cfg -f target/stm32h7x.cfg \
        -c "program ${elf} verify reset exit" \
        || echo -e "${RED}Flash failed (is OpenOCD installed and board connected?)${NC}"
}

case "${BUILD_TYPE,,}" in
    clean)
        do_clean
        ;;
    flash)
        do_flash
        ;;
    *)
        do_build "${BUILD_TYPE}"
        ;;
esac
