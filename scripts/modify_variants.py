import os
import shutil
from os.path import join

Import("env")

PROJECT_DIR = env.subst("$PROJECT_DIR")

platform = env.PioPlatform()
FRAMEWORK_DIR = platform.get_package_dir("framework-arduinoespressif32")
variants_dir = join(FRAMEWORK_DIR, "variants")

def modify_variants():
    print("Patching variant file pins_arduino.h")
    src_file = join(PROJECT_DIR, "boards", "variants", "esp32s3", "pins_arduino.h")
    dst_file = join(FRAMEWORK_DIR, "variants", "esp32s3", "pins_arduino.h")
    try:
        shutil.copy(src_file, dst_file)
    except:
        src_file = join(PROJECT_DIR, "lib", "OpenIndus", "boards", "variants", "esp32s3", "pins_arduino.h")
        shutil.copy(src_file, dst_file)
        
modify_variants()