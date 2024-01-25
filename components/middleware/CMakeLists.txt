set(COMPONENT_SRCS
    "Bus/BusIO.cpp"
    "Bus/BusRs.cpp"
    "Bus/BusCan.cpp"
    "Encoder/Encoder.cpp"
    "DigitalInput/DigitalInput.cpp"
    "DigitalOutput/DigitalOutput.cpp"
    "Motor/Motor.cpp"
    "Motor/MotorBLDC.cpp"
    "Motor/MotorStepper.cpp"
    "Usb/UsbConsole.cpp"
    "Usb/UsbSerial.cpp"
    "FlashLoader/FlashLoader.cpp"
)

set(COMPONENT_INCLUDE_DIRS
    "Bus"
    "Encoder"
    "DigitalInput"
    "DigitalOutput"
    "AnalogInput"
    "AnalogOutput"
    "Motor"
    "Usb"
    "FlashLoader"
)

set(COMPONENT_PRIV_REQUIRES
    "esp_adc_cal"
    "console"
    "app_update"
    "esp_rom"
)

set(COMPONENT_REQUIRES
    "drivers"
)

idf_component_register(
    SRCS ${COMPONENT_SRCS}
    INCLUDE_DIRS ${COMPONENT_INCLUDE_DIRS}
    PRIV_REQUIRES ${COMPONENT_PRIV_REQUIRES}
    REQUIRES ${COMPONENT_REQUIRES}
)