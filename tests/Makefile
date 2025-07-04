# Makefile for Python tests

# Variables
PYTHON = python3
VENV_DIR = venv
VENV_ACTIVATE = $(VENV_DIR)/bin/activate

.PHONY: test setup clean help

# Default target
test: setup
	@echo "Running tests..."
	. $(VENV_ACTIVATE) && python3 -m pytest test_system.py
	. $(VENV_ACTIVATE) && python3 -m pytest test_digital_io.py
	. $(VENV_ACTIVATE) && python3 -m pytest test_rs.py
	. $(VENV_ACTIVATE) && python3 -m pytest test_can.py

# Setup
setup: $(VENV_ACTIVATE)
	@echo "Setting up..."
	@bash config_can.sh up
	. $(VENV_ACTIVATE) && pip install --upgrade pip
	. $(VENV_ACTIVATE) && pip install -r requirements.txt

$(VENV_ACTIVATE):
	@echo "Creating virtual environment..."
	$(PYTHON) -m venv $(VENV_DIR)

# Clean
clean:
	@echo "Cleaning up..."
	@bash config_can.sh down
	rm -rf __pycache__/ .pytest_cache/ $(VENV_DIR)/

# Help
help:
	@echo "Available commands:"
	@echo "  test       - Run Python tests (default)"
	@echo "  setup      - Create virtual environment and install dependencies"
	@echo "  clean      - Remove temporary files and virtual environment"
	@echo "  help       - Show this help message"
