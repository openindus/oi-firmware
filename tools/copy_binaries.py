import os
import shutil
from os.path import join

Import("env")

PROJECT_DIR = env.subst("$PROJECT_DIR")
BUILD_DIR = env.subst("$BUILD_DIR")
PIOENV = env.subst("$PIOENV")

def copy_binaries(source, target, env):
    with open("version.txt", "r") as f:
        oi_tag = f.readline().strip()

    oi_bin_dir = join(PROJECT_DIR, ".oi", "bin", "firmware-" + oi_tag)

    # Create OpenIndus bin directory
    try:
        os.makedirs(oi_bin_dir, exist_ok = True)
        print("Directory %s created successfully" % oi_bin_dir)
    except OSError as error:
        print("Directory %s can not be created" % oi_bin_dir)

    # Move binaries
    try:
        shutil.copyfile(join(BUILD_DIR, "firmware.bin"), join(oi_bin_dir, PIOENV + "-firmware-" + oi_tag + ".bin"))
        shutil.copyfile(join(BUILD_DIR, "ota_data_initial.bin"), join(oi_bin_dir, PIOENV + "-ota_data_initial-" + oi_tag + ".bin"))
        shutil.copyfile(join(BUILD_DIR, "bootloader.bin"), join(oi_bin_dir, PIOENV + "-bootloader-" + oi_tag + ".bin"))
        shutil.copyfile(join(BUILD_DIR, "partitions.bin"), join(oi_bin_dir, PIOENV + "-partitions-" + oi_tag + ".bin"))
        print("Copy binaries to the %s directory" % oi_bin_dir)
    except:
        print("Cannot copy binaries")


env.AddPostAction("buildprog", copy_binaries)
