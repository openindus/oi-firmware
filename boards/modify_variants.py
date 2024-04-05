import os
import shutil
from os.path import join

Import("env")

PROJECT_DIR = env.subst("$PROJECT_DIR")

platform = env.PioPlatform()
FRAMEWORK_DIR = platform.get_package_dir("framework-arduinoespressif32")
variants_dir = join(FRAMEWORK_DIR, "variants")

def find_files(filename, search_path):
    result = []
    # Walking top-down from the root
    for root, dir, files in os.walk(search_path):
        if filename in files:
            result.append(os.path.join(root, filename))
    return result

def modify_variants():
    print("Patching variant file pins_arduino.h")
    dst_file = join(variants_dir, "esp32s3", "pins_arduino.h")
    src_file = find_files("pins_arduino.h", PROJECT_DIR)
    shutil.copy(src_file[0], dst_file)

        
modify_variants()