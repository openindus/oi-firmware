"""
@file test_can.py
@brief Test script for CAN functions
@author Kevin Lefeuvre (kevin.lefeuvre@openindus.com)
@copyright (c) [2025] OpenIndus, Inc. All rights reserved.
@see https://openindus.com
"""

import pytest
import time
import can
from utils.cli import CLI

@pytest.fixture
def module_type(request):
    return request.config.getoption("--module-type")

@pytest.fixture
def cli(dut, module_type):
    return CLI(dut, module_type)

@pytest.fixture
def can_interface():
    """Setup PEAK CAN interface"""
    interface = "can0"
    
    try:
        bus = can.interface.Bus(channel=interface, interface='socketcan')
        yield bus
        
    except Exception as e:
        pytest.skip(f"Failed to create CAN interface: {e}")
    
    finally:
        bus.shutdown()

def can_send(bus, can_id, data_bytes, extended=False):
    """Send CAN message using python-can"""
    try:
        msg = can.Message(
            arbitration_id=can_id,
            data=data_bytes,
            is_extended_id=extended
        )
        bus.send(msg, timeout=2.0)
        return True
    except Exception as e:
        print(f"Failed to send CAN message: {e}")
        return False

def can_receive(bus, timeout=2.0):
    """Receive CAN messages using python-can"""
    messages = []
    start_time = time.time()
    
    try:
        while time.time() - start_time < timeout:
            msg = bus.recv(timeout=0.1)  # Short timeout for polling
            if msg:
                messages.append({
                    'id': msg.arbitration_id,
                    'length': len(msg.data),
                    'data': msg.data.hex().upper(),
                    'extended': msg.is_extended_id
                })
    except Exception:
        pass
    
    return messages

def test_can_init(dut, cli):
    """Test CAN initialization"""
    
    # Wait for prompt
    dut.expect(cli.prompt, timeout=10)
    
    # Initialize CAN with default parameters (1 Mbps)
    dut.write("can-begin")
    dut.expect("CAN initialized", timeout=5)
    dut.expect(cli.prompt, timeout=5)
    
    # Test available (should be 0 initially)
    dut.write("can-available")
    response = dut.expect(r"(\d+)\s*\n" + cli.prompt, timeout=5)
    available = int(response.group(1))
    assert available == 0, f"Expected 0 messages available, got {available}"
    
    # End CAN communication
    dut.write("can-end")
    dut.expect("CAN communication ended", timeout=5)
    dut.expect(cli.prompt, timeout=5)

def test_can_init_with_parameters(dut, cli):
    """Test CAN initialization with specific parameters"""
    
    dut.expect(cli.prompt, timeout=10)
    
    # Initialize CAN with 500 kbps
    dut.write("can-begin --baudrate 500000")
    dut.expect("CAN initialized", timeout=5)
    dut.expect("baudrate=500000", timeout=5)
    dut.expect(cli.prompt, timeout=5)
    
    # Test extended mode
    dut.write("can-begin --baudrate 250000 --extended")
    dut.expect("CAN initialized", timeout=5)
    dut.expect("mode=Extended", timeout=5)
    dut.expect(cli.prompt, timeout=5)
    
    # End CAN communication
    dut.write("can-end")
    dut.expect("CAN communication ended", timeout=5)
    dut.expect(cli.prompt, timeout=5)

def test_can_write_standard_frame(dut, cli, can_interface):
    """Test CAN standard frame writing"""
    
    dut.expect(cli.prompt, timeout=10)
    
    # Initialize CAN with 500 kbps to match interface
    dut.write("can-begin --baudrate 500000")
    dut.expect("CAN initialized", timeout=5)
    dut.expect(cli.prompt, timeout=5)
    
    # Send standard CAN frame
    test_id = 0x123
    test_data = "AABBCCDD"
    
    dut.write(f"can-write --id {test_id} --data {test_data}")
    dut.expect("CAN message sent", timeout=5)
    dut.expect(f"ID=0x{test_id:X}", timeout=5)
    dut.expect("Extended=No", timeout=5)
    dut.expect(cli.prompt, timeout=5)
    
    # Wait a moment for transmission
    time.sleep(0.5)
    
    # Receive messages directly
    messages = can_receive(can_interface, 2.0)
    
    # Check if message was received
    found = False
    for msg in messages:
        if msg['id'] == test_id and test_data.upper() == msg['data']:
            found = True
            break
    
    assert found, f"Expected CAN message with ID 0x{test_id:X} and data {test_data} not received"
    
    # End CAN communication
    dut.write("can-end")
    dut.expect("CAN communication ended", timeout=5)
    dut.expect(cli.prompt, timeout=5)

def test_can_write_extended_frame(dut, cli, can_interface):
    """Test CAN extended frame writing"""
    
    dut.expect(cli.prompt, timeout=10)
    
    # Initialize CAN with extended mode
    dut.write("can-begin --baudrate 500000 --extended")
    dut.expect("CAN initialized", timeout=5)
    dut.expect(cli.prompt, timeout=5)
    
    # Send extended CAN frame
    test_id = 0x18FF1234
    test_data = "DEADBEEF"
    
    dut.write(f"can-write --id {test_id} --data {test_data} --extended")
    dut.expect("CAN message sent", timeout=5)
    dut.expect(f"ID=0x{test_id:X}", timeout=5)
    dut.expect("Extended=Yes", timeout=5)
    dut.expect(cli.prompt, timeout=5)
    
    # Wait and receive messages
    time.sleep(0.5)
    messages = can_receive(can_interface, 2.0)
    
    # Check if message was received
    found = False
    for msg in messages:
        if msg['id'] == test_id and msg['extended'] and test_data.upper() == msg['data']:
            found = True
            break
    
    assert found, f"Expected extended CAN message with ID 0x{test_id:X} not received"
    
    # End CAN communication
    dut.write("can-end")
    dut.expect("CAN communication ended", timeout=5)
    dut.expect(cli.prompt, timeout=5)

@pytest.mark.skip(reason="RTR frame testing temporarily disabled")
def test_can_write_rtr_frame(dut, cli, can_interface):
    """Test CAN RTR frame writing"""
    
    dut.expect(cli.prompt, timeout=10)
    
    # Initialize CAN
    dut.write("can-begin --baudrate 500000")
    dut.expect("CAN initialized", timeout=5)
    dut.expect(cli.prompt, timeout=5)
    
    # Send RTR frame
    test_id = 0x456
    
    dut.write(f"can-write --id {test_id} --rtr")
    dut.expect("CAN message sent", timeout=5)
    dut.expect(f"ID=0x{test_id:X}", timeout=5)
    dut.expect("RTR=Yes", timeout=5)
    dut.expect("Size=0", timeout=5)
    dut.expect(cli.prompt, timeout=5)
    
    # End CAN communication
    dut.write("can-end")
    dut.expect("CAN communication ended", timeout=5)
    dut.expect(cli.prompt, timeout=5)

def test_can_read_loopback(dut, cli, can_interface):
    """Test CAN reading by sending from PEAK interface"""
    
    dut.expect(cli.prompt, timeout=10)
    
    # Initialize CAN
    dut.write("can-begin --baudrate 500000")
    dut.expect("CAN initialized", timeout=5)
    dut.expect(cli.prompt, timeout=5)
    
    # Send CAN message from PEAK interface
    test_id = 0x20A
    test_data = bytes([0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08])
    
    success = can_send(can_interface, test_id, test_data)
    assert success, "Failed to send CAN message from PEAK interface"
    
    time.sleep(0.5)  # Give time for transmission
    
    # Check available messages
    dut.write("can-available")
    response = dut.expect(r"(\d+)\s*\n" + cli.prompt, timeout=5)
    available = int(response.group(1))
    
    assert available > 0, f"Expected > 0 messages, got {available}"
    
    # Read the message
    dut.write("can-read")
    dut.expect("CAN message received", timeout=5)
    dut.expect(f"ID=0x{test_id:X}", timeout=5)
    dut.expect("Size=8", timeout=5)
    dut.expect(cli.prompt, timeout=5)
    
    # End CAN communication
    dut.write("can-end")
    dut.expect("CAN communication ended", timeout=5)
    dut.expect(cli.prompt, timeout=5)

@pytest.mark.skip(reason="CAN filter testing temporarily disabled")
def test_can_filters(dut, cli):
    """Test CAN filter configuration"""
    
    dut.expect(cli.prompt, timeout=10)
    
    # Initialize CAN
    dut.write("can-begin --baudrate 500000")
    dut.expect("CAN initialized", timeout=5)
    dut.expect(cli.prompt, timeout=5)
    
    # Set standard filter
    mask = 0x7FF
    filter_id = 0x123
    
    dut.write(f"can-set-standard-filter --mask {mask} --filter {filter_id}")
    dut.expect("Standard CAN filter set", timeout=5)
    dut.expect(f"mask=0x{mask:04X}", timeout=5)
    dut.expect(f"filter=0x{filter_id:04X}", timeout=5)
    dut.expect(cli.prompt, timeout=5)
    
    # Set extended filter
    ext_mask = 0x1FFFFFFF
    ext_filter = 0x18FF1234
    
    dut.write(f"can-set-extended-filter --mask {ext_mask} --filter {ext_filter}")
    dut.expect("Extended CAN filter set", timeout=5)
    dut.expect(f"mask=0x{ext_mask:08X}", timeout=5)
    dut.expect(f"filter=0x{ext_filter:08X}", timeout=5)
    dut.expect(cli.prompt, timeout=5)
    
    # End CAN communication
    dut.write("can-end")
    dut.expect("CAN communication ended", timeout=5)
    dut.expect(cli.prompt, timeout=5)

def test_can_multiple_messages(dut, cli, can_interface):
    """Test sending and receiving multiple CAN messages"""
    
    dut.expect(cli.prompt, timeout=10)
    
    # Initialize CAN
    dut.write("can-begin --baudrate 500000")
    dut.expect("CAN initialized", timeout=5)
    dut.expect(cli.prompt, timeout=5)
    
    # Send multiple messages from PEAK
    test_messages = [
        (0x30A, bytes([0x01, 0x01, 0x01, 0x01, 0x02, 0x02, 0x02, 0x02])),
        (0x40A, bytes([0x03, 0x03, 0x03, 0x03, 0x04, 0x04, 0x04, 0x04])),
        (0x50A, bytes([0xAA, 0xBB, 0xCC, 0xDD]))
    ]
    
    for msg_id, data in test_messages:
        success = can_send(can_interface, msg_id, data)
        assert success, f"Failed to send CAN message ID 0x{msg_id:X}"
        time.sleep(0.1)
    
    time.sleep(1.0)  # Give time for all messages to be received
    
    # Check available count
    dut.write("can-available")
    response = dut.expect(r"(\d+)\s*\n" + cli.prompt, timeout=5)
    available = int(response.group(1))
    
    assert available >= len(test_messages), f"Expected >= {len(test_messages)} messages, got {available}"
    
    # Read messages one by one
    for i in range(min(available, len(test_messages))):
        dut.write("can-read")
        dut.expect("CAN message received", timeout=5)
        dut.expect(cli.prompt, timeout=5)
    
    # End CAN communication
    dut.write("can-end")
    dut.expect("CAN communication ended", timeout=5)
    dut.expect(cli.prompt, timeout=5)