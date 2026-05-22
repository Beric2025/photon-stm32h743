# Makefile wrapper for STM32 Embedded Framework
#
# Usage:
#   make              # Release build
#   make debug        # Debug build
#   make h750-debug   # STM32H750 Debug build
#   make clean        # Clean all build directories
#   make flash        # Build and flash via openocd

BUILD_TYPE ?= release

.PHONY: all debug release h750-debug clean flash help

all: release

debug:
	cmake --preset debug -S .
	cmake --build build/debug -j$$(nproc 2>/dev/null || echo 4)

release:
	cmake --preset release -S .
	cmake --build build/release -j$$(nproc 2>/dev/null || echo 4)

h750-debug:
	cmake --preset h750-debug -S .
	cmake --build build/h750-debug -j$$(nproc 2>/dev/null || echo 4)

clean:
	rm -rf build

flash:
	openocd -f interface/stlink.cfg -f target/stm32h7x.cfg \
		-c "program build/release/stm32-embedded-framework.elf verify reset exit"

help:
	@echo "STM32 Embedded Framework Build"
	@echo ""
	@echo "  make              Release build"
	@echo "  make debug        Debug build"
	@echo "  make h750-debug   STM32H750 Debug build"
	@echo "  make clean        Clean all build directories"
	@echo "  make flash        Flash release build to target"
	@echo "  make help         Show this help"
