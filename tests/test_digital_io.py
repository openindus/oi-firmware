"""
@file test_digital_io.py
@brief Test script for Digital I/O functions
@author Kevin Lefeuvre (kevin.lefeuvre@openindus.com)
@copyright (c) [2025] OpenIndus, Inc. All rights reserved.
@see https://openindus.com
"""

import pytest
import time
from utils.cli import CLI

@pytest.fixture
def module_type(request):
    return request.config.getoption("--module-type")

@pytest.fixture
def cli(dut, module_type):
    return CLI(dut, module_type)

@pytest.fixture
def dout_module_id(request, cli):
    return cli.get_module_id(request.param)

@pytest.fixture
def din_module_id(request, cli):
    return cli.get_module_id(request.param)

@pytest.fixture
def ain_module_id(request, cli):
    return cli.get_module_id(request.param)

@pytest.mark.parametrize("dout_module_id,dout_num,din_module_id,din_num", [
    ("core", 1, "discrete", 1), # DOUT 1 -> DIN 1
    ("core", 2, "discrete", 2), # DOUT 2 -> DIN 2
    ("core", 3, "discrete", 3), # DOUT 3 -> DIN 3
    ("core", 4, "discrete", 4), # DOUT 4 -> DIN 4
    ("discrete", 1, "mixed", 1), # DOUT 1 -> DIN 1
    ("discrete", 2, "mixed", 2), # DOUT 2 -> DIN 2
    ("discrete", 3, "mixed", 3), # DOUT 3 -> DIN 3
    ("discrete", 4, "mixed", 4), # DOUT 4 -> DIN 4
    ("mixed", 1, "core", 1), # DOUT 1 -> DIN 1
    ("mixed", 2, "core", 2), # DOUT 2 -> DIN 2
    ("mixed", 3, "core", 3), # DOUT 3 -> DIN 3
    ("mixed", 4, "core", 4), # DOUT 4 -> DIN 4
], indirect=["dout_module_id", "din_module_id"])
def test_digital_read_write_toggle(dut, cli, dout_module_id, dout_num, din_module_id, din_num):
    """Test digital read/write and toggle functionality"""

    dut.expect(cli.prompt, timeout=10)

    # Set output mode to digital to avoid PWM mode
    cli.send_command(dout_module_id, f"output-mode -d {dout_num} -m digital")

    # Set DOUT HIGH
    cli.send_command(dout_module_id, f"digital-write {dout_num} 1")

    # Check DIN HIGH
    cli.send_command(din_module_id, f"digital-read {din_num}", wait_for_prompt=False)
    response = dut.expect(r"(\d+)\s*\n" + cli.prompt, timeout=5)
    read_value = int(response.group(1))
    assert read_value == 1, f"Expected 1, got {read_value}"

    # Set DOUT LOW
    cli.send_command(dout_module_id, f"digital-write {dout_num} 0")

    # Check DIN LOW
    cli.send_command(din_module_id, f"digital-read {din_num}", wait_for_prompt=False)
    response = dut.expect(r"(\d+)\s*\n" + cli.prompt, timeout=5)
    read_value = int(response.group(1))
    assert read_value == 0, f"Expected 0, got {read_value}"

    # Read initial DIN value (should be 0)
    initial_value = read_value

    # Toggle DOUT
    cli.send_command(dout_module_id, f"toggle-output -d {dout_num}")

    # Read DIN after toggle
    cli.send_command(din_module_id, f"digital-read {din_num}", wait_for_prompt=False)
    response = dut.expect(r"(\d+)\s*\n" + cli.prompt, timeout=5)
    toggled_value = int(response.group(1))
    assert toggled_value == (1 - initial_value), f"Expected DIN {din_num} to toggle from {initial_value} to {1-initial_value}, got {toggled_value}"

    # Toggle again
    cli.send_command(dout_module_id, f"toggle-output -d {dout_num}")

    # Read DIN after second toggle
    cli.send_command(din_module_id, f"digital-read {din_num}", wait_for_prompt=False)
    response = dut.expect(r"(\d+)\s*\n" + cli.prompt, timeout=5)
    toggled_back_value = int(response.group(1))
    assert toggled_back_value == initial_value, f"Expected DIN {din_num} to return to {initial_value}, got {toggled_back_value}"

@pytest.mark.parametrize("dout_module_id,dout_num,din_module_id,din_num,mode", [
    ("core", 1, "core", 1, "rising"), # DOUT 1 -> DIN 1 rising edge
    ("core", 2, "core", 2, "falling"), # DOUT 2 -> DIN 2 falling edge  
    ("core", 3, "core", 3, "change"), # DOUT 3 -> DIN 3 both edges
    ("discrete", 4, "discrete", 4, "rising"), # DOUT 4 -> DIN 4 rising edge
    ("discrete", 1, "discrete", 1, "falling"), # DOUT 1 -> DIN 1 falling edges
    ("discrete", 2, "discrete", 2, "change"), # DOUT 2 -> DIN 2 both edges
    ("mixed", 3, "mixed", 3, "rising"), # DOUT 3 -> DIN 3 rising edge
    ("mixed", 4, "mixed", 4, "falling"), # DOUT 4 -> DIN 4 falling edge
    ("mixed", 1, "mixed", 1, "change"), # DOUT 1 -> DIN 1 both edges
], indirect=["dout_module_id", "din_module_id"])
def test_digital_interrupt(dut, cli, dout_module_id, dout_num, din_module_id, din_num, mode):
    """Test interrupt functionality on digital inputs"""
    
    dut.expect(cli.prompt, timeout=10)

    # Set output mode to digital to avoid PWM mode
    cli.send_command(dout_module_id, f"output-mode -d {dout_num} -m digital")
    
    # Ensure DOUT is initially LOW
    cli.send_command(dout_module_id, f"digital-write {dout_num} 0")
    time.sleep(0.1)
    
    # Attach interrupt
    cli.send_command(din_module_id, f"attach-interrupt -d {din_num} -m {mode}")
    
    # Generate interrupt based on mode
    if mode == "rising" or mode == "change":
        # Generate rising edge: LOW -> HIGH
        cli.send_command(dout_module_id, f"digital-write {dout_num} 1", wait_for_prompt=False)

        # Wait for interrupt message
        try:
            response = dut.expect(r"Interrupt triggered on DIN_(\d+)", timeout=35)
            triggered_din = int(response.group(1))
            dut.write("") # Send empty command to recover the prompt
            dut.expect(cli.prompt, timeout=5)
        except:
            pytest.fail("Interrupt not triggered within timeout")
    
    if mode == "falling" or mode == "change":
        # For falling edge, first ensure HIGH state
        if mode == "falling":
            cli.send_command(dout_module_id, f"digital-write {dout_num} 1")
            time.sleep(0.1)
        
        # Generate falling edge: HIGH -> LOW
        cli.send_command(dout_module_id, f"digital-write {dout_num} 0", wait_for_prompt=False)
        
        # Wait for interrupt message
        try:
            response = dut.expect(r"Interrupt triggered on DIN_(\d+)", timeout=5)
            triggered_din = int(response.group(1))
            dut.write("") # Send empty command to recover the prompt
            dut.expect(cli.prompt, timeout=5)
        except:
            pytest.fail("Interrupt not triggered within timeout")
    
    # Detach interrupt
    cli.send_command(din_module_id, f"detach-interrupt {din_num}")

    # Verify interrupt is detached by generating an edge that should not trigger
    cli.send_command(dout_module_id, f"digital-write {dout_num} 1")
    time.sleep(0.5) # Wait to ensure no interrupt is triggered
    cli.send_command(dout_module_id, f"digital-write {dout_num} 0")

@pytest.mark.parametrize("dout_module_id,dout_num,ain_module_id,ain_num,duty_cycle", [
    ("discrete", 5, "core", 1, 25.0), # DOUT 1 -> AIN 1 with 25% duty cycle
    ("discrete", 5, "core", 1, 50.0), # DOUT 1 -> AIN 1 with 50% duty cycle
    ("discrete", 5, "core", 1, 75.0), # DOUT 1 -> AIN 1 with 75% duty cycle
    ("discrete", 6, "core", 2, 25.0), # DOUT 2 -> AIN 2 with 25% duty cycle
    ("discrete", 6, "core", 2, 50.0), # DOUT 2 -> AIN 2 with 50% duty cycle
    ("discrete", 6, "core", 2, 75.0), # DOUT 2 -> AIN 2 with 75% duty cycle
    # ("discrete", 5, "discrete", 1, 25.0), # DOUT 1 -> AIN 1 with 25% duty cycle
    # ("discrete", 5, "discrete", 1, 50.0), # DOUT 1 -> AIN 1 with 50% duty cycle
    # ("discrete", 5, "discrete", 1, 75.0), # DOUT 1 -> AIN 1 with 75% duty cycle
    # ("discrete", 6, "discrete", 2, 25.0), # DOUT 2 -> AIN 2 with 25% duty cycle
    # ("discrete", 6, "discrete", 2, 50.0), # DOUT 2 -> AIN 2 with 50% duty cycle
    # ("discrete", 6, "discrete", 2, 75.0), # DOUT 2 -> AIN 2 with 75% duty cycle
], indirect=["dout_module_id", "ain_module_id"])
def test_set_pwm_analog_read(dut, cli, dout_module_id, dout_num, ain_module_id, ain_num, duty_cycle):
    """Test PWM functionality on digital outputs and analog reading"""
    
    dut.expect(cli.prompt, timeout=10)

    # Set output mode to PWM
    cli.send_command(dout_module_id, f"output-mode -d {dout_num} -m pwm")

    # Set PWM frequency to 1000 Hz
    pwm_freq = 1000
    cli.send_command(dout_module_id, f"set-pwm-frequency -d {dout_num} -f {pwm_freq}")

    # Set PWM duty cycle
    cli.send_command(dout_module_id, f"set-pwm-duty-cycle -d {dout_num} -c {duty_cycle}")

    # Wait a bit for signal to stabilize
    time.sleep(0.5)

    # Take 100 analog samples, spaced by 1 PWM period
    samples = 100
    pwm_period = 1.0 / pwm_freq
    voltage_readings = []
    for i in range(samples):
        cli.send_command(ain_module_id, f"analog-read {ain_num} 3", wait_for_prompt=False)
        response = dut.expect(r"(\d+\.\d+)", timeout=5)
        voltage = float(response.group(1))
        dut.expect(cli.prompt, timeout=5)
        voltage_readings.append(voltage)
        time.sleep(pwm_period)
    
    average_voltage = sum(voltage_readings) / len(voltage_readings)

    # Calculate expected voltage based on duty cycle
    expected_voltage = 24.0 * (duty_cycle / 100.0)
    tolerance = 2.0  # 2V tolerance for HV inputs
    voltage_diff = abs(average_voltage - expected_voltage)
    assert voltage_diff <= tolerance, (
        f"Voltage difference {voltage_diff:.3f}V exceeds tolerance {tolerance}V. "
        f"Expected: {expected_voltage:.3f}V, Got: {average_voltage:.3f}V"
    )

    # Set duty cycle to 0% to turn off PWM
    cli.send_command(dout_module_id, f"set-pwm-duty-cycle -d {dout_num} -c 0.0")

    # Reset output mode to default
    cli.send_command(dout_module_id, f"output-mode -d {dout_num} -m digital")