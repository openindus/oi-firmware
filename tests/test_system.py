import pytest

def test_system_initialization_and_startup(dut):
    """Test system initialization and startup - ensure system initializes and main task starts correctly."""

    # Check if the system initialized successfully
    dut.expect("System initialized successfully", timeout=30)
    
    # Check if the main task started successfully
    dut.expect("Main task started successfully", timeout=30)

def test_console_startup(dut):
    """Test console startup - ensure console starts correctly."""

    # Check if the prompt is available
    dut.write("")  # Send empty command to get prompt
    dut.expect("Core>", timeout=5)
