from os.path import join, isfile

Import("env")

env.Execute("cd libraries/Arduino && git restore ./ && git apply ../../tools/Arduino.patch && cd ../..")
