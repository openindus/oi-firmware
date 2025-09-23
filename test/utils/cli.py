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

    def send_command(self, module_id, command, wait_for_prompt=True, timeout=5):
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
                
                self.dut.write(f"get-slave-id {module_type_cfg} {serial_number}")
                response = self.dut.expect(r"Slave ID: (\d+)", timeout=5)
                return int(response.group(1))
        
        pytest.skip(f"Module '{module_type}' not found in configuration")