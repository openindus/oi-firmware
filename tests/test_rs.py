"""
@file test_rs.py
@brief Test script for RS485/232 CLI functions
@author Kevin Lefeuvre (kevin.lefeuvre@openindus.com)
@copyright (c) [2025] OpenIndus, Inc. All rights reserved.
@see https://openindus.com
"""

import pytest
import serial
import time
from utils.cli import CLI

@pytest.fixture
def module_type(request):
    return request.config.getoption("--module-type")

@pytest.fixture
def cli(dut, module_type):
    return CLI(dut, module_type)

@pytest.fixture
def ftdi_port():
    """FTDI adapter for external RS communication"""
    return serial.Serial(
        port="/dev/ttyUSB_FTDI",
        baudrate=9600,
        timeout=2.0,
        bytesize=serial.EIGHTBITS,
        parity=serial.PARITY_NONE,
        stopbits=serial.STOPBITS_ONE
    )

def test_rs_init_and_basic_commands(dut, cli):
    """Test RS initialization and basic commands"""
    
    # Wait for prompt
    dut.expect(cli.prompt, timeout=10)
    
    # Initialize RS with default parameters
    dut.write("rs-begin")
    dut.expect("RS initialized", timeout=5)
    dut.expect(cli.prompt, timeout=5)
    
    # Test available for write
    dut.write("rs-available-for-write")
    response = dut.expect(r"(\d+)\s*\n" + cli.prompt, timeout=5)
    available = int(response.group(1))
    assert available > 0, f"Expected available bytes > 0, got {available}"
    
    # Test available (should be 0 initially)
    dut.write("rs-available")
    response = dut.expect(r"(\d+)\s*\n" + cli.prompt, timeout=5)
    available = int(response.group(1))
    assert available == 0, f"Expected 0 bytes available, got {available}"

def test_rs_write_byte(dut, cli, ftdi_port):
    """Test RS byte writing"""
    
    dut.expect(cli.prompt, timeout=10)
    
    # Initialize RS
    dut.write("rs-begin --baudrate 9600 --mode RS485")
    dut.expect("RS initialized", timeout=5)
    dut.expect(cli.prompt, timeout=5)
    
    # Clear FTDI buffer
    ftdi_port.reset_input_buffer()
    
    # Write a test byte
    test_byte = 0xAA
    dut.write(f"rs-write-byte --byte {test_byte}")
    dut.expect("Wrote 1 byte", timeout=5)
    dut.expect(cli.prompt, timeout=5)
    
    # Read from FTDI
    time.sleep(0.5)  # Give time for transmission
    received = ftdi_port.read(1)
    
    assert len(received) == 1, f"Expected 1 byte, got {len(received)}"
    assert received[0] == test_byte, f"Expected 0x{test_byte:02X}, got 0x{received[0]:02X}"

def test_rs_write_buffer(dut, cli, ftdi_port):
    """Test RS buffer writing"""
    
    dut.expect(cli.prompt, timeout=10)
    
    # Initialize RS
    dut.write("rs-begin --baudrate 9600 --mode RS485")
    dut.expect("RS initialized", timeout=5)
    dut.expect(cli.prompt, timeout=5)
    
    # Clear FTDI buffer
    ftdi_port.reset_input_buffer()
    
    # Write test string
    test_string = "Hello RS485"
    dut.write(f'rs-write-buffer --data "{test_string}"')
    dut.expect(f"Wrote {len(test_string)} bytes", timeout=5)
    dut.expect(cli.prompt, timeout=5)
    
    # Read from FTDI
    time.sleep(0.5)
    received = ftdi_port.read(len(test_string))
    received_str = received.decode('utf-8', errors='ignore')
    
    assert received_str == test_string, f"Expected '{test_string}', got '{received_str}'"

def test_rs_read_loopback(dut, cli, ftdi_port):
    """Test RS reading by sending from FTDI"""
    
    dut.expect(cli.prompt, timeout=10)
    
    # Initialize RS
    dut.write("rs-begin --baudrate 9600 --mode RS485")
    dut.expect("RS initialized", timeout=5)
    dut.expect(cli.prompt, timeout=5)
    
    # Send data from FTDI
    test_data = b"Test123"
    ftdi_port.write(test_data)
    ftdi_port.flush()
    
    time.sleep(0.5)  # Give time for transmission
    
    # Check available bytes
    dut.write("rs-available")
    response = dut.expect(r"(\d+)\s*\n" + cli.prompt, timeout=5)
    available = int(response.group(1))
    
    assert available == len(test_data), f"Expected {len(test_data)} bytes, got {available}"
    
    # Read the data
    dut.write(f"rs-read-buffer --size {available}")
    dut.expect("Read", timeout=5)
    dut.expect(cli.prompt, timeout=5)

def test_rs_single_byte_read(dut, cli, ftdi_port):
    """Test single byte reading"""
    
    dut.expect(cli.prompt, timeout=10)
    
    # Initialize RS
    dut.write("rs-begin --baudrate 9600 --mode RS485")
    dut.expect("RS initialized", timeout=5)
    dut.expect(cli.prompt, timeout=5)
    
    # Send single byte from FTDI
    test_byte = 0x55
    ftdi_port.write(bytes([test_byte]))
    ftdi_port.flush()
    
    time.sleep(0.5)
    
    # Read single byte
    dut.write("rs-read")
    dut.expect(f"0x{test_byte:02X}", timeout=5)
    dut.expect(cli.prompt, timeout=5)

def test_rs_end(dut, cli):
    """Test RS termination"""
    
    dut.expect(cli.prompt, timeout=10)
    
    # Initialize RS first
    dut.write("rs-begin")
    dut.expect("RS initialized", timeout=5)
    dut.expect(cli.prompt, timeout=5)
    
    # End RS communication
    dut.write("rs-end")
    dut.expect("RS communication ended", timeout=5)
    dut.expect(cli.prompt, timeout=5)
