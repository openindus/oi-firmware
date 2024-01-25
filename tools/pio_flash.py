import sys
from os.path import isfile, join

from platformio.util import get_serial_ports

Import("env")

# print(env.Dump())
env = DefaultEnvironment()
platform = env.PioPlatform()
board = env.BoardConfig()
mcu = board.get("build.mcu", "esp32")

if "flash" in COMMAND_LINE_TARGETS:
    env.Append(
        UPLOADFILE = join("$PROJECT_DIR", ".oi", "bin", "oi-firmware-1.0.2","stepperves2_firmware-1.0.2.bin")
    )
    env.Replace(
        UPLOADERFLAGS=[
            "--chip", mcu,
            "--port", '"$UPLOAD_PORT"',
            "--baud", "115200",
            "--befor", "no_reset", 
            "--after", "hard_reset",
            "write_flash", "-z",
            "--flash_mode", "${__get_board_flash_mode(__env__)}",
            "--flash_freq", "${__get_board_f_flash(__env__)}",
            "--flash_size", board.get("upload.flash_size", "detect"),
            "--help"
        ],
        UPLOADCMD='"$PYTHONEXE" "$UPLOADER" $UPLOADERFLAGS $ESP32_APP_OFFSET $UPLOADFILE'
    )

def flash(target, source, env):
    env.AutodetectUploadPort()
    env.Execute("$UPLOADCMD")

env.AddCustomTarget(
    name="flash",
    dependencies=None,
    actions=flash,
    title=None,
    description="Flash the program through the OpenIndus Bus",
    always_build=True
)