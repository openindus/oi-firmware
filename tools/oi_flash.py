import os
import time
import argparse
import subprocess
from oi_console import Console

if __name__ == '__main__':

    parser = argparse.ArgumentParser(description='Flash program')
    parser.add_argument('-p', '--port', type=str, help='Serial port', required=True)
    parser.add_argument('-f', '--file', type=argparse.FileType('r'), help='Binary file', required=True)
    parser.add_argument('-n', '--serial-number', type=str, help='Serial number', required=True)
    args = parser.parse_args()

    cli = Console(port=args.port)
    cli.connect()
    cli.wait_for_prompt('Core>')
    cli.command('log disable', ret=False)
    cli.command('program ' + args.serial_number, ret=False)
    cli.disconnect()

    time.sleep(3)

    subprocess.run([
        "esptool.py", 
        "--port", args.port, 
        "--baud", "115200", 
        "--before", "no_reset",
        "--no-stub",
        # "--trace",
        "write_flash", 
        "0x110000", 
        args.file.name])
