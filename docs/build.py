#!/usr/bin/env python3
# coding=utf-8

import os
import sys
import subprocess


if __name__ == "__main__":

    # Clean project
    subprocess.call("make clean", shell=True)
    subprocess.call("rm -rf tmp", shell=True)

    # Generate Doxyfile
    # subprocess.call("doxygen -g tmp/Doxyfile", shell=True)

    # Create source code documentation
    subprocess.call("doxygen doxygen/Doxyfile", shell=True)

    # Build documentation
    subprocess.call("make html", shell=True)