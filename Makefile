# Makefile for OI-Firmware

.ONESHELL:
SHELL := /bin/bash

# Default variables
IDF_PATH ?= $(HOME)/esp/esp-idf
IDF_EXPORT_SCRIPT = $(IDF_PATH)/export.sh
BUILD_DIR = build
PROJECT_NAME = OI-Firmware

# Setup ESP-IDF environment
get-idf:
	@echo "source $(IDF_EXPORT_SCRIPT)"
	@source $(IDF_EXPORT_SCRIPT)

# Project build
build: get-idf
	@idf.py build

# Complete cleanup
clean:
	@rm -rf $(BUILD_DIR)

# Full cleanup
fullclean:
	@rm -rf $(BUILD_DIR)
	@rm -f sdkconfig

# Flash firmware
flash: get-idf
	@idf.py flash

# Serial monitoring
monitor: get-idf
	@idf.py monitor

# Flash and monitor
flash-monitor: get-idf
	@idf.py flash monitor

# Menu configuration
menuconfig: get-idf
	@idf.py menuconfig

# Examples
build-examples: get-idf
	find $(EXAMPLES_DIR) -name "*.cpp" | while read -r f; do \
		echo "Build $$f"; \
		cp "$$f" main/main.cpp; \
		cp sdkconfig.ci.defaults.core sdkconfig; \
		idf.py reconfigure; \
		idf.py build || { echo "Error: Build failed for $$f"; exit 1; }; \
	done

# Help
help:
	@echo "Makefile for OI-Firmware"
	@echo ""
	@echo "Environment variables:"
	@echo "  IDF_PATH          Path to ESP-IDF (default: ~/esp/esp-idf)"
	@echo ""
	@echo "Targets:"
	@echo "  build             Build firmware (default)"
	@echo "  clean             Clean build files"
	@echo "  fullclean         Full cleanup"
	@echo "  flash             Flash firmware"
	@echo "  monitor           Serial monitoring"
	@echo "  flash-monitor     Flash and monitor"
	@echo "  menuconfig        Menu configuration"
	@echo "  build-examples    Build all examples (use EXAMPLES_DIR variable)"
	@echo ""
	@echo "Other:"
	@echo "  help              Show this help"

# Default target
.DEFAULT_GOAL := build

# Targets that don't correspond to files
.PHONY: build clean fullclean flash monitor flash-monitor menuconfig build-examples help
