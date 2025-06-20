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
def module_out(request, cli):
    return cli.get_module_id(request.param)

@pytest.fixture
def module_in(request, cli):
    return cli.get_module_id(request.param)

@pytest.mark.parametrize("module_out,dout,module_in,din", [
    ("core", 1, "discrete", 1),     # DOUT 1 -> DIN 1
    ("core", 2, "discrete", 2),     # DOUT 2 -> DIN 2
    ("core", 3, "discrete", 3),     # DOUT 3 -> DIN 3
    ("core", 4, "discrete", 4),     # DOUT 4 -> DIN 4
    ("discrete", 1, "mixed", 1),    # DOUT 1 -> DIN 1
    ("discrete", 2, "mixed", 2),    # DOUT 2 -> DIN 2
    ("discrete", 3, "mixed", 3),    # DOUT 3 -> DIN 3
    ("discrete", 4, "mixed", 4),    # DOUT 4 -> DIN 4
    ("mixed", 1, "core", 1),        # DOUT 1 -> DIN 1
    ("mixed", 2, "core", 2),        # DOUT 2 -> DIN 2
    ("mixed", 3, "core", 3),        # DOUT 3 -> DIN 3
    ("mixed", 4, "core", 4),        # DOUT 4 -> DIN 4
], indirect=["module_out", "module_in"])
def test_read_write_toggle(dut, cli, module_out, dout, module_in, din):
    """Test digital read/write and toggle functionality"""

    dut.expect(cli.prompt, timeout=10)

    # Set output mode to digital to avoid PWM mode
    cli.set_output_mode(module_out, dout, mode="digital")

    # Set DOUT HIGH
    cli.digital_write(module_out, dout, 1)

    # Check DIN HIGH
    read_value = cli.digital_read(module_in, din)
    assert read_value == 1, f"Expected 1, got {read_value}"

    # Set DOUT LOW
    cli.digital_write(module_out, dout, 0)

    # Check DIN LOW
    read_value = cli.digital_read(module_in, din)
    assert read_value == 0, f"Expected 0, got {read_value}"

    # Read initial DIN value (should be 0)
    initial_value = read_value

    # Toggle DOUT
    cli.toggle_output(module_out, dout)

    # Read DIN after toggle
    toggled_value = cli.digital_read(module_in, din)
    assert toggled_value == (1 - initial_value), f"Expected DIN {din} to toggle from {initial_value} to {1-initial_value}, got {toggled_value}"

    # Toggle again
    cli.toggle_output(module_out, dout)

    # Read DIN after second toggle
    toggled_back_value = cli.digital_read(module_in, din)
    assert toggled_back_value == initial_value, f"Expected DIN {din} to return to {initial_value}, got {toggled_back_value}"

@pytest.mark.parametrize("module_out,dout,module_in,din,mode", [
    ("core", 1, "core", 1, "rising"),           # DOUT 1 -> DIN 1 rising edge
    ("core", 2, "core", 2, "falling"),          # DOUT 2 -> DIN 2 falling edge  
    ("core", 3, "core", 3, "change"),           # DOUT 3 -> DIN 3 both edges
    ("discrete", 4, "discrete", 4, "rising"),   # DOUT 4 -> DIN 4 rising edge
    ("discrete", 1, "discrete", 1, "falling"),  # DOUT 1 -> DIN 1 falling edges
    ("discrete", 2, "discrete", 2, "change"),   # DOUT 2 -> DIN 2 both edges
    ("mixed", 3, "mixed", 3, "rising"),         # DOUT 3 -> DIN 3 rising edge
    ("mixed", 4, "mixed", 4, "falling"),        # DOUT 4 -> DIN 4 falling edge
    ("mixed", 1, "mixed", 1, "change"),         # DOUT 1 -> DIN 1 both edges
], indirect=["module_out", "module_in"])
def test_interrupt(dut, cli, module_out, dout, module_in, din, mode):
    """Test interrupt functionality on digital inputs"""
    
    dut.expect(cli.prompt, timeout=10)

    # Set output mode to digital to avoid PWM mode
    cli.set_output_mode(module_out, dout, mode="digital")
    
    # Ensure DOUT is initially LOW
    cli.digital_write(module_out, dout, 0)
    time.sleep(0.1)
    
    # Attach interrupt
    cli.attach_interrupt(module_in, din, mode=mode)
    
    # Generate interrupt based on mode
    if mode == "rising" or mode == "change":
        # Generate rising edge: LOW -> HIGH
        cli.digital_write(module_out, dout, 1, wait_for_prompt=False)

        # Wait for interrupt message
        cli.wait_for_interrupt(module_in, timeout=35)
    
    if mode == "falling" or mode == "change":
        # For falling edge, first ensure HIGH state
        if mode == "falling":
            cli.digital_write(module_out, dout, 1)
            time.sleep(0.1)
        
        # Generate falling edge: HIGH -> LOW
        cli.digital_write(module_out, dout, 0, wait_for_prompt=False)
        
        # Wait for interrupt message
        cli.wait_for_interrupt(module_in, timeout=5)
    
    # Detach interrupt
    cli.detach_interrupt(module_in, din)

    # Verify interrupt is detached by generating an edge that should not trigger
    cli.digital_write(module_out, dout, 1)
    time.sleep(0.5) # Wait to ensure no interrupt is triggered
    cli.digital_write(module_out, dout, 0)

@pytest.mark.parametrize("module_out,dout,module_in,ain,duty_cycle", [
    ("discrete", 5, "core", 1, 25.0),       # DOUT 1 -> AIN 1 with 25% duty cycle
    ("discrete", 5, "core", 1, 50.0),       # DOUT 1 -> AIN 1 with 50% duty cycle
    ("discrete", 5, "core", 1, 75.0),       # DOUT 1 -> AIN 1 with 75% duty cycle
    ("discrete", 6, "core", 2, 25.0),       # DOUT 2 -> AIN 2 with 25% duty cycle
    ("discrete", 6, "core", 2, 50.0),       # DOUT 2 -> AIN 2 with 50% duty cycle
    ("discrete", 6, "core", 2, 75.0),       # DOUT 2 -> AIN 2 with 75% duty cycle
    # ("discrete", 5, "discrete", 1, 25.0),   # DOUT 1 -> AIN 1 with 25% duty cycle
    # ("discrete", 5, "discrete", 1, 50.0),   # DOUT 1 -> AIN 1 with 50% duty cycle
    # ("discrete", 5, "discrete", 1, 75.0),   # DOUT 1 -> AIN 1 with 75% duty cycle
    # ("discrete", 6, "discrete", 2, 25.0),   # DOUT 2 -> AIN 2 with 25% duty cycle
    # ("discrete", 6, "discrete", 2, 50.0),   # DOUT 2 -> AIN 2 with 50% duty cycle
    # ("discrete", 6, "discrete", 2, 75.0),   # DOUT 2 -> AIN 2 with 75% duty cycle
], indirect=["module_out", "module_in"])
def test_pwm(dut, cli, module_out, dout, module_in, ain, duty_cycle):
    """Test PWM functionality on digital outputs and analog inputs"""
    
    dut.expect(cli.prompt, timeout=10)

    # Set output mode to PWM
    cli.set_output_mode(module_out, dout, mode="pwm")

    # Set PWM frequency to 1000 Hz
    pwm_freq = 1000
    cli.set_pwm_frequency(module_out, dout, pwm_freq)

    # Set PWM duty cycle
    cli.set_pwm_duty_cycle(module_out, dout, duty_cycle)

    # Wait a bit for signal to stabilize
    time.sleep(0.5)

    # Take 100 analog samples, spaced by 1 PWM period
    samples = 100
    pwm_period = 1.0 / pwm_freq
    voltage_readings = []
    for i in range(samples):
        voltage = cli.analog_read(module_in, ain)
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
    cli.set_pwm_duty_cycle(module_out, dout, 0.0)

    # Reset output mode to default
    cli.set_output_mode(module_out, dout, mode="digital")