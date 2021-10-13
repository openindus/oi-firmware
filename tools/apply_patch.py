from os.path import join, isfile

Import("env")

env.Execute("cd libraries/Arduino && git apply ../../Arduino.patch && cd ../..")
