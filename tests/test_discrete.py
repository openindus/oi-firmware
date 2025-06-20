import pytest
import time
import json
import os

@pytest.fixture
def discrete_id(dut):
    """Get module ID for Discrete board from configuration."""
    # Load config.json
    config_path = os.path.join(os.path.dirname(__file__), "config.json")
    with open(config_path, 'r') as f:
        config = json.load(f)
    
    # Find discrete module
    discrete_module = None
    for module in config["test_bench"]["modules"]:
        if module["name"] == "discrete":
            discrete_module = module
            break
    
    if not discrete_module:
        pytest.skip("Discrete module not found in configuration")
    
    board_type = discrete_module["type"]
    serial_number = discrete_module["serial_number"]
    dut.expect("Core>", timeout=10)
    dut.write(f"get-slave-id {board_type} {serial_number}")
    response = dut.expect(r"Slave ID: (\d+)", timeout=5)
    slave_id = int(response.group(1))
    return slave_id

@pytest.mark.parametrize("dout,din", [
    (1, 1),  # DOUT 1 -> DIN 1
    (2, 2),  # DOUT 2 -> DIN 2
    (3, 3),  # DOUT 3 -> DIN 3
    (4, 4),  # DOUT 4 -> DIN 4
    (5, 5),  # DOUT 5 -> DIN 5
    (6, 6),  # DOUT 6 -> DIN 6
    (7, 7),  # DOUT 7 -> DIN 7
    (8, 8),  # DOUT 8 -> DIN 8
    (7, 9),  # DOUT 9 -> DIN 10
    (8, 10),  # DOUT 10 -> DIN 11
])
def test_digital_write_read(dut, discrete_id, dout, din):
    """Test writing to DOUT and reading from DIN."""

    print(f"Testing DOUT {dout} -> DIN {din} with module ID {discrete_id}")

    print("Waiting for Core> prompt...")
    dut.expect("Core>", timeout=10)
    
    # Set DOUT HIGH
    dut.write(f"digital-write -i {discrete_id} {dout} 1")
    dut.expect("Core>", timeout=5)
    
    # Check DIN
    dut.write(f"digital-read {din} -i {discrete_id}")
    response = dut.expect(r"(\d+)\s*\nCore>", timeout=5)
    read_value = int(response.group(1))
    assert read_value == 1, f"Expected 1, got {read_value}"
    print(f"DOUT {dout} HIGH -> DIN {din} = {read_value}")
    
    # Set DOUT LOW
    dut.write(f"digital-write -i {discrete_id} {dout} 0")
    dut.expect("Core>", timeout=5)
    
    # Check DIN
    dut.write(f"digital-read {din} -i {discrete_id}")
    response = dut.expect(r"(\d+)\s*\nCore>", timeout=5)
    read_value = int(response.group(1))
    assert read_value == 0, f"Expected 0, got {read_value}"
    print(f"DOUT {dout} LOW -> DIN {din} = {read_value}")


@pytest.mark.parametrize("dout,din,mode", [
    (1, 1, "rising"),   # DOUT 1 -> DIN 1 rising edge
    (2, 2, "falling"),  # DOUT 2 -> DIN 2 falling edge  
    (3, 3, "change"),   # DOUT 3 -> DIN 3 both edges
    (7, 9, "rising"),   # DOUT 7 -> DIN 9 rising edge
    (8, 10, "falling"), # DOUT 8 -> DIN 10 falling edge
])
def test_digital_interrupt(dut, discrete_id, dout, din, mode):
    """Test digital interrupt functionality."""
    
    print(f"Testing interrupt DOUT {dout} -> DIN {din} with mode {mode} and module ID {discrete_id}")
    
    print("Waiting for Core> prompt...")
    dut.expect("Core>", timeout=10)
    
    # Ensure DOUT is initially LOW
    dut.write(f"digital-write -i {discrete_id} {dout} 0")
    dut.expect("Core>", timeout=5)
    time.sleep(0.1)
    
    # Attach interrupt
    dut.write(f"attach-interrupt -i {discrete_id} -d {din} -m {mode}")
    dut.expect("Core>", timeout=5)
    print(f"Attached interrupt on DIN {din} with mode {mode}")
    
    # Generate interrupt based on mode
    if mode == "rising" or mode == "change":
        # Generate rising edge: LOW -> HIGH
        print(f"Generating rising edge on DOUT {dout}")
        dut.write(f"digital-write -i {discrete_id} {dout} 1")
        
        # Wait for interrupt message
        try:
            response = dut.expect(r"Interrupt triggered on DIN_(\d+)", timeout=3)
            triggered_din = int(response.group(1))
            assert triggered_din == din, f"Expected interrupt on DIN {din}, got DIN {triggered_din}"
            print(f"✓ Rising edge interrupt triggered successfully on DIN {din}")
        except:
            pytest.fail(f"Rising edge interrupt not triggered on DIN {din}")
            
        dut.expect("Core>", timeout=5)
        
    if mode == "falling" or mode == "change":
        # For falling edge, first ensure HIGH state
        if mode == "falling":
            dut.write(f"digital-write -i {discrete_id} {dout} 1")
            dut.expect("Core>", timeout=5)
            time.sleep(0.1)
        
        # Generate falling edge: HIGH -> LOW
        print(f"Generating falling edge on DOUT {dout}")
        dut.write(f"digital-write -i {discrete_id} {dout} 0")
        
        # Wait for interrupt message
        try:
            response = dut.expect(r"Interrupt triggered on DIN_(\d+)", timeout=3)
            triggered_din = int(response.group(1))
            assert triggered_din == din, f"Expected interrupt on DIN {din}, got DIN {triggered_din}"
            print(f"✓ Falling edge interrupt triggered successfully on DIN {din}")
        except:
            pytest.fail(f"Falling edge interrupt not triggered on DIN {din}")
            
        dut.expect("Core>", timeout=5)
    
    # Detach interrupt
    dut.write(f"detach-interrupt -i {discrete_id} -d {din}")
    dut.expect("Core>", timeout=5)
    print(f"Detached interrupt on DIN {din}")
    
    # Verify interrupt is detached by generating an edge that should not trigger
    print("Verifying interrupt is detached...")
    dut.write(f"digital-write -i {discrete_id} {dout} 1")
    dut.expect("Core>", timeout=5)
    time.sleep(0.5)  # Wait to ensure no interrupt is triggered
    
    dut.write(f"digital-write -i {discrete_id} {dout} 0")
    dut.expect("Core>", timeout=5)
    print("✓ No interrupt triggered after detach - test passed")
