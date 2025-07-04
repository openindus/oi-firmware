"""
@file test_system.py
@brief Test script for System functions
@author Kevin Lefeuvre (kevin.lefeuvre@openindus.com)
@copyright (c) [2025] OpenIndus, Inc. All rights reserved.
@see https://openindus.com
"""

import pytest
import json
import os

def test_init_and_start(dut):
    """Test the initialization and starting of the system"""

    # Check if the hardware initialized successfully
    dut.expect("Hardware initialized successfully", timeout=30)

    # Check if auto ID assignment completed successfully
    dut.expect("Auto ID modules done", timeout=30)

    # Check if no boot error is detected
    dut.expect("No boot error detected", timeout=30)
    
    # Check if the main task started successfully
    dut.expect("Main task started successfully", timeout=30)

    # Check if the prompt is available
    dut.write("")  # Send empty command to get prompt
    dut.expect("Core>", timeout=5)

def test_discover_slaves(dut):
    """Test discover slaves command"""

    # Load expected slaves from config.json
    config_path = os.path.join(os.path.dirname(__file__), "config.json")
    with open(config_path, 'r') as f:
        config = json.load(f)
    
    # Extract expected slaves
    expected_slaves = [
        {
            "type": module["type"],
            "sn": module["serial_number"]
        }
        for module in config["test_bench"]["modules"]
    ]
    
    # Wait for prompt before sending command
    dut.expect("Core>", timeout=10)
    
    # Send discover-slaves command
    dut.write("discover-slaves")
    
    # Extract JSON response using regex
    response = dut.expect(r'(\[{"id":[^\]]+\])', timeout=10)
    json_response = response.group(1)  # Use group(1) to get the first capture group
    
    # Parse returned JSON
    discovered_slaves = json.loads(json_response)
    
    # Validation: verify that list matches our pre-registered list
    assert isinstance(discovered_slaves, list), "Response should be a JSON array"
    
    # Sort lists by type and serial number for comparison
    expected_sorted = sorted(expected_slaves, key=lambda x: (x["type"], x["sn"]))
    discovered_sorted = sorted(
        [ {"type": s["type"], "sn": s["sn"]} for s in discovered_slaves ],
        key=lambda x: (x["type"], x["sn"]))
    
    # Verify number of slaves
    assert len(discovered_sorted) == len(expected_sorted), f"Expected {len(expected_sorted)} slaves, got {len(discovered_sorted)}"
    
    # Verify each slave (excluding ID check)
    for i, (expected, discovered) in enumerate(zip(expected_sorted, discovered_sorted)):
        assert discovered["type"] == expected["type"], f"Slave {i}: Type mismatch - expected {expected['type']}, got {discovered['type']}"
        assert discovered["sn"] == expected["sn"], f"Slave {i}: Serial number mismatch - expected {expected['sn']}, got {discovered['sn']}"

def test_ping_modules(dut):
    """Test ping command"""

    # Load expected modules from config.json
    config_path = os.path.join(os.path.dirname(__file__), "config.json")
    with open(config_path, 'r') as f:
        config = json.load(f)
    modules = config["test_bench"]["modules"]

    # Wait for prompt before sending commands
    dut.expect("Core>", timeout=10)

    for module in modules:
        module_type = module["type"]
        module_sn = module["serial_number"]
        dut.write(f"ping {module_type} {module_sn}")
        dut.expect(rf"Ping module: {module_sn} time: \d+ us", timeout=5)

def test_get_slave_info(dut):
    """Test get-slave-info command"""

    # Load expected modules from config.json
    config_path = os.path.join(os.path.dirname(__file__), "config.json")
    with open(config_path, 'r') as f:
        config = json.load(f)
    modules = config["test_bench"]["modules"]

    # Wait for prompt before sending commands
    dut.expect("Core>", timeout=10)

    for module in modules:
        module_type = module["type"]
        module_sn = module["serial_number"]
        
        # Test get-slave-info command with board type flag
        dut.write(f"get-slave-info {module_type} {module_sn} -t")
        dut.expect(rf"{module_type}", timeout=5)
        
        # Test get-slave-info command with serial number flag
        dut.write(f"get-slave-info {module_type} {module_sn} -n")
        dut.expect(rf"{module_sn}", timeout=5)
        
        # Test get-slave-info command with software version flag
        dut.write(f"get-slave-info {module_type} {module_sn} -s")
        dut.expect(r"\d+\.\d+\.\d+", timeout=5)  # Expected format: X.Y.Z
        
        # Test get-slave-info command with hardware version flag
        dut.write(f"get-slave-info {module_type} {module_sn} -h")
        dut.expect(r"\d+", timeout=5)  # Hardware variant as number
        
        # Test get-slave-info command with timestamp flag
        dut.write(f"get-slave-info {module_type} {module_sn} -d")
        dut.expect(r"\d+", timeout=5)  # Unix timestamp as number