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
	@echo "=== Setting up ESP-IDF environment ==="
	@echo "source $(IDF_EXPORT_SCRIPT)"
	@source $(IDF_EXPORT_SCRIPT)

# Project build
build: get-idf
	@echo "=== Building project ==="
	@idf.py build

# Complete cleanup
clean:
	@echo "=== Project cleanup ==="
	@rm -rf $(BUILD_DIR)

# Full cleanup
fullclean:
	@echo "=== Full project cleanup ==="
	@rm -rf $(BUILD_DIR)
	@rm -f sdkconfig

# Flash firmware
flash: get-idf
	@echo "=== Firmware flashing ==="
	@idf.py flash

# Serial monitoring
monitor: get-idf
	@echo "=== Serial monitoring ==="
	@idf.py monitor

# Flash and monitor
flash-monitor: get-idf
	@echo "=== Flash and monitor ==="
	@idf.py flash monitor

# Menu configuration
menuconfig: get-idf
	@echo "=== Menu configuration ==="
	@idf.py menuconfig

# Run tests
test:
	@echo "=== Running tests ==="
	@cd test
	@pip install -r requirements.txt
	@python -m pytest

# Build documentation
docs:
	@echo "=== Documentation build ==="
	@cd docs && make html

# Help
help:
	@echo "Makefile for OI-Firmware"
	@echo ""
	@echo "Environment variables:"
	@echo "  IDF_PATH          Path to ESP-IDF (default: ~/esp/esp-idf)"
	@echo ""
	@echo "Main targets:"
	@echo "  build             Build firmware (default)"
	@echo "  clean             Clean build files"
	@echo "  fullclean         Full cleanup"
	@echo "  flash             Flash firmware"
	@echo "  monitor           Serial monitoring"
	@echo "  flash-monitor     Flash and monitor"
	@echo "  menuconfig        Menu configuration"
	@echo ""
	@echo "Tests:"
	@echo "  test              Run tests"
	@echo ""
	@echo "Documentation:"
	@echo "  docs              Build documentation"
	@echo ""
	@echo "Other:"
	@echo "  help              Show this help"

# Default target
.DEFAULT_GOAL := build

# Targets that don't correspond to files
.PHONY: build clean fullclean flash monitor flash-monitor menuconfig \
        test docs help
