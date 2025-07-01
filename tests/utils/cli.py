import json
import os
import pytest

class CLI:
    """Command Line Interface for interacting with modules"""
    def __init__(self, dut, module_type):
        self.dut = dut
        self.type = module_type

    @property
    def prompt(self):
        prompts = {
            "discrete": "Discrete>",
            "core": "Core>",
        }
        return prompts.get(self.type, f"{self.type.capitalize()}>")

    def _send_command(self, module_id, command, wait_for_prompt=True, timeout=5):
        """Send command with module ID if needed"""
        if module_id is not None:
            # Add module ID to command for modules that have an ID
            if " -i " not in command:
                command += f" -i {module_id}"
        
        self.dut.write(command)
        
        if wait_for_prompt:
            self.dut.expect(self.prompt, timeout=timeout)

    def get_module_id(self, module_type=None):
        """Get module ID from configuration. Return None if it's a 'core'."""
        if module_type == "core":
            return None

        config_path = os.path.join(os.path.dirname(__file__), "../config.json")
        with open(config_path, 'r') as f:
            config = json.load(f)
        
        for module in config["test_bench"]["modules"]:
            if module["name"] == module_type:
                module_type_cfg = module["type"]
                serial_number = module["serial_number"]
                
                self.dut.expect(self.prompt, timeout=10)
                self.dut.write(f"get-slave-id {module_type_cfg} {serial_number}")
                response = self.dut.expect(r"Slave ID: (\d+)", timeout=5)
                return int(response.group(1))
        
        pytest.skip(f"Module '{module_type}' not found in configuration")
    
    # === Digital I/O Commands ===
    
    def set_output_mode(self, module_id, pin, mode="digital"):
        """Set output mode (digital/pwm)"""
        self._send_command(module_id, f"output-mode -d {pin} -m {mode}")
    
    def digital_write(self, module_id, pin, value, wait_for_prompt=True):
        """Write digital value to output pin"""
        self._send_command(module_id, f"digital-write {pin} {value}", wait_for_prompt=wait_for_prompt)

    def digital_read(self, module_id, pin):
        """Read digital value from input pin"""
        self._send_command(module_id, f"digital-read {pin}", wait_for_prompt=False)
        response = self.dut.expect(r"(\d+)\s*\n" + self.prompt, timeout=5)
        return int(response.group(1))

    def toggle_output(self, module_id, pin):
        """Toggle output pin state"""
        self._send_command(module_id, f"toggle-output -d {pin}")

    def set_pwm_frequency(self, module_id, pin, frequency):
        """Set PWM frequency for output pin"""
        self._send_command(module_id, f"set-pwm-frequency -d {pin} -f {frequency}")

    def set_pwm_duty_cycle(self, module_id, pin, duty_cycle):
        """Set PWM duty cycle for output pin"""
        self._send_command(module_id, f"set-pwm-duty-cycle -d {pin} -c {duty_cycle}")

    def attach_interrupt(self, module_id, pin, mode="rising"):
        """Attach interrupt to input pin"""
        self._send_command(module_id, f"attach-interrupt -d {pin} -m {mode}")

    def detach_interrupt(self, module_id, pin):
        """Detach interrupt from input pin"""
        self._send_command(module_id, f"detach-interrupt {pin}")

    def wait_for_interrupt(self, module_id, timeout=3):
        """Wait for interrupt message and return the triggered DIN number"""
        try:
            response = self.dut.expect(r"Interrupt triggered on DIN_(\d+)", timeout=timeout)
            triggered_din = int(response.group(1))
            self.dut.write("") # Send empty command to recover the prompt
            self.dut.expect(self.prompt, timeout=5)
            return triggered_din
        except:
            return None
    
    # === Analog Commands ===

    def analog_read(self, module_id, pin, channel=3):
        """Read analog value from input pin"""
        self._send_command(module_id, f"analog-read {pin} {channel}", wait_for_prompt=False)
        response = self.dut.expect(r"(\d+\.\d+)", timeout=5)
        voltage = float(response.group(1))
        self.dut.expect(self.prompt, timeout=5)
        return voltage
