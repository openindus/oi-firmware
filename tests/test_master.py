import pytest
import json
import re
import os

def test_discover_slaves(dut):
    """Test discover slaves command - ensure the command returns the expected slaves list."""
    
    # Load expected slaves from config.json
    config_path = os.path.join(os.path.dirname(__file__), "config.json")
    with open(config_path, 'r') as f:
        config = json.load(f)
    
    # Extract expected slaves
    expected_slaves = [
        {
            "id": module["expected_id"],
            "type": module["type"],
            "sn": module["serial_number"]
        }
        for module in config["test_bench"]["modules"]
    ]
    print(f"Expected slaves from config: {expected_slaves}")
    
    # Wait for prompt before sending command
    print("Waiting for Core> prompt...")
    dut.expect("Core>", timeout=10)
    
    # Send discover-slaves command
    print("Sending discover-slaves command...")
    dut.write("discover-slaves")
    
    # Wait and read response
    try:
        # Extract JSON response using regex
        response = dut.expect(r'(\[{"id":[^\]]+\])', timeout=10)
        json_response = response.group(1)  # Use group(1) to get the first capture group
        print(f"Received response: {json_response}")
        
        # Parse returned JSON
        discovered_slaves = json.loads(json_response)
        print(f"Parsed slaves: {discovered_slaves}")
        
        # Validation: verify that list matches our pre-registered list
        assert isinstance(discovered_slaves, list), "Response should be a JSON array"
        
        # Sort lists by type and serial number for comparison
        expected_sorted = sorted(expected_slaves, key=lambda x: (x["type"], x["sn"]))
        discovered_sorted = sorted(discovered_slaves, key=lambda x: (x["type"], x["sn"]))
        
        # Verify number of slaves
        assert len(discovered_sorted) == len(expected_sorted), f"Expected {len(expected_sorted)} slaves, got {len(discovered_sorted)}"
        
        # Verify each slave (excluding ID check)
        for i, (expected, discovered) in enumerate(zip(expected_sorted, discovered_sorted)):
            assert discovered["type"] == expected["type"], f"Slave {i}: Type mismatch - expected {expected['type']}, got {discovered['type']}"
            assert discovered["sn"] == expected["sn"], f"Slave {i}: Serial number mismatch - expected {expected['sn']}, got {discovered['sn']}"
        
        print("Discover slaves test passed - all expected slaves found and validated")
        
    except Exception as e:
        print(f"Discover slaves test failed: {str(e)}")
        raise
