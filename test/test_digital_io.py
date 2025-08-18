"""
@file test_digital_io.py
@brief Test script for Digital I/O functions
@author Kevin Lefeuvre (kevin.lefeuvre@openindus.com)
@copyright (c) [2025] OpenIndus, Inc. All rights reserved.
@see https://openindus.com
"""

import pytest
import time
import json
import os
from utils.cli import CLI

@pytest.fixture
def module_type(request):
    return request.config.getoption("--module-type")

@pytest.fixture
def cli(dut, module_type):
    return CLI(dut, module_type)

def load_config():
    """Load test configuration from config.json"""
    config_path = os.path.join(os.path.dirname(__file__), "config.json")
    with open(config_path, 'r') as f:
        return json.load(f)

def test_digital_read_write_toggle(dut, cli):
    """Test digital read/write and toggle functionality"""

    # Load wiring configuration
    config = load_config()
    digital_io_wiring = config["test_bench"]["wiring"]["digital_io"]

    # Wait for prompt before starting tests
    dut.expect(cli.prompt, timeout=10)

    for wiring in digital_io_wiring:
        dout_module_id = cli.get_module_id(wiring["module_dout"])
        dout_num = wiring["dout"]
        din_module_id = cli.get_module_id(wiring["module_din"])
        din_num = wiring["din"]

        print(f"Testing DOUT {wiring['module_dout']}:{dout_num} -> DIN {wiring['module_din']}:{din_num}")

        # Set output mode to digital to avoid PWM mode
        cli.send_command(dout_module_id, f"output-mode -d {dout_num} -m digital")

        # Set DOUT HIGH
        cli.send_command(dout_module_id, f"digital-write {dout_num} 1")

        # Check DIN HIGH
        cli.send_command(din_module_id, f"digital-read {din_num}", wait_for_prompt=False)
        response = dut.expect(r"(\d+)\s*\n" + cli.prompt, timeout=5)
        read_value = int(response.group(1))
        assert read_value == 1, f"Expected 1, got {read_value} for {wiring['module_dout']}:{dout_num} -> {wiring['module_din']}:{din_num}"

        # Set DOUT LOW
        cli.send_command(dout_module_id, f"digital-write {dout_num} 0")

        # Check DIN LOW
        cli.send_command(din_module_id, f"digital-read {din_num}", wait_for_prompt=False)
        response = dut.expect(r"(\d+)\s*\n" + cli.prompt, timeout=5)
        read_value = int(response.group(1))
        assert read_value == 0, f"Expected 0, got {read_value} for {wiring['module_dout']}:{dout_num} -> {wiring['module_din']}:{din_num}"

        # Read initial DIN value (should be 0)
        initial_value = read_value

        # Toggle DOUT
        cli.send_command(dout_module_id, f"toggle-output -d {dout_num}")

        # Read DIN after toggle
        cli.send_command(din_module_id, f"digital-read {din_num}", wait_for_prompt=False)
        response = dut.expect(r"(\d+)\s*\n" + cli.prompt, timeout=5)
        toggled_value = int(response.group(1))
        assert toggled_value == (1 - initial_value), f"Expected DIN {din_num} to toggle from {initial_value} to {1-initial_value}, got {toggled_value} for {wiring['module_dout']}:{dout_num} -> {wiring['module_din']}:{din_num}"

        # Toggle again
        cli.send_command(dout_module_id, f"toggle-output -d {dout_num}")

        # Read DIN after second toggle
        cli.send_command(din_module_id, f"digital-read {din_num}", wait_for_prompt=False)
        response = dut.expect(r"(\d+)\s*\n" + cli.prompt, timeout=5)
        toggled_back_value = int(response.group(1))
        assert toggled_back_value == initial_value, f"Expected DIN {din_num} to return to {initial_value}, got {toggled_back_value} for {wiring['module_dout']}:{dout_num} -> {wiring['module_din']}:{din_num}"

def test_digital_interrupt(dut, cli):
    """Test interrupt functionality on digital inputs"""
    
    # Load wiring configuration
    config = load_config()
    digital_io_wiring = config["test_bench"]["wiring"]["digital_io"]
    
    # Test only the first few connections for each mode to avoid excessive test time
    test_connections = digital_io_wiring[:3]  # Test first 3 connections
    
    # Test all interrupt modes
    interrupt_modes = ["rising", "falling", "change"]
    
    # Wait for prompt before starting tests
    dut.expect(cli.prompt, timeout=10)
    
    for mode in interrupt_modes:
        for wiring in test_connections:
            # Only test if both modules are the same (loopback on same module)
            if wiring["module_dout"] == wiring["module_din"]:
                dout_module_id = cli.get_module_id(wiring["module_dout"])
                dout_num = wiring["dout"]
                din_module_id = cli.get_module_id(wiring["module_din"])
                din_num = wiring["din"]

                print(f"Testing interrupt {mode} on {wiring['module_dout']}:{dout_num} -> {wiring['module_din']}:{din_num}")

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
                        pytest.fail(f"Interrupt not triggered within timeout for {mode} on {wiring['module_dout']}:{dout_num}")
                
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
                        pytest.fail(f"Interrupt not triggered within timeout for {mode} on {wiring['module_dout']}:{dout_num}")
                
                # Detach interrupt
                cli.send_command(din_module_id, f"detach-interrupt {din_num}")

                # Verify interrupt is detached by generating an edge that should not trigger
                cli.send_command(dout_module_id, f"digital-write {dout_num} 1")
                time.sleep(0.5) # Wait to ensure no interrupt is triggered
                cli.send_command(dout_module_id, f"digital-write {dout_num} 0")

def test_set_pwm_analog_read(dut, cli):
    """Test PWM functionality on digital outputs and analog reading"""
    
    # Load wiring configuration
    config = load_config()
    analog_io_wiring = config["test_bench"]["wiring"]["analog_io"]
    
    # Test different duty cycles
    duty_cycles = [25.0, 50.0, 75.0]
    
    # Wait for prompt before starting tests
    dut.expect(cli.prompt, timeout=10)
    
    for duty_cycle in duty_cycles:
        for wiring in analog_io_wiring:
            dout_module_id = cli.get_module_id(wiring["module_dout"])
            dout_num = wiring["dout"]
            ain_module_id = cli.get_module_id(wiring["module_ain"])
            ain_num = wiring["ain"]

            print(f"Testing PWM {duty_cycle}% on {wiring['module_dout']}:{dout_num} -> AIN {wiring['module_ain']}:{ain_num}")

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
                f"Voltage difference {voltage_diff:.3f}V exceeds tolerance {tolerance}V for {wiring['module_dout']}:{dout_num} -> {wiring['module_ain']}:{ain_num}. "
                f"Expected: {expected_voltage:.3f}V, Got: {average_voltage:.3f}V"
            )

            # Set duty cycle to 0% to turn off PWM
            cli.send_command(dout_module_id, f"set-pwm-duty-cycle -d {dout_num} -c 0.0")

            # Reset output mode to default
            cli.send_command(dout_module_id, f"output-mode -d {dout_num} -m digital")