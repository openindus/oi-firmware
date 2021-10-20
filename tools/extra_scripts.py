import os
from os.path import join, isfile

Import("env")

PROJECT_DIR = env.subst("$PROJECT_DIR")

platform = env.PioPlatform()
board = env.BoardConfig()
mcu = board.get("build.mcu", "esp32")

oi_flash_program ="app_flash_%s.bin" % mcu

env.Append(
    FLASH_EXTRA_IMAGES=[
        (
            "0x3C0000",
            join("$PROJECT_DIR", "bin", oi_flash_program),
        ),
    ],
)
