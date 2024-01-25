import os, time
import serial
from serial.serialutil import SerialException

class Console:
    def __init__(self, port='/dev/ttyUSB0', baudrate=115200, timeout=3):
        self.port = port
        self.baudrate = baudrate
        self.timeout = timeout
        self.serial = None

    def _reset_chip(self):
        self.serial.setRTS(True)
        self.serial.setDTR(False)
        time.sleep(0.1)
        self.serial.setDTR(True)
        time.sleep(0.1)
        self.serial.setRTS(False)
        self.serial.setDTR(False)

    def _reset_buffer(self):
        self.serial.reset_input_buffer()
        self.serial.reset_output_buffer()

    def connect(self, reset=True):
        if not self.serial:
            self.serial = serial.Serial(port=self.port, baudrate=self.baudrate, timeout=self.timeout)
            if reset:
                self._reset_chip()

    def disconnect(self):
        if self.serial and self.serial.isOpen():
            self.serial.close()

    def command(self, cmd, ret=True):
        try:
            self._reset_buffer()
            self.serial.write(cmd.encode() + b'\r\n')
            self.serial.readline().decode()
            if ret:
                response = self.serial.readline().decode()
                while not response:
                    response = self.serial.readline().decode()
                return response.strip()
        except SerialException as e:
            raise Exception(f"Error reading from serial port: {e}")

    def wait_for_prompt(self, prompt, timeout=3):
        start_time = time.time()
        response = ''
        while prompt not in response:
            response += self.serial.read(self.serial.inWaiting()).decode()
            if time.time() - start_time > timeout:
                raise TimeoutError("Timeout while waiting for prompt.")
        return response.strip()

    def read(self):
        return self.serial.read().decode()

    def readline(self):
        return self.serial.readline().decode()

    def write(self, data):
        self.serial.write(data)

    def flush(self):
        self.serial.flush()
