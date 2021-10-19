Import("env")

try:
    env.Execute("cd libraries/Arduino && git restore ./ && git apply ../../tools/Arduino.patch && cd ../..")
except:
    None # patch already applied or git not available
