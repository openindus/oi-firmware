set(COMPONENT_SRCS
    "Bus/BusIO.cpp"
    "Bus/BusRs.cpp"
    "Bus/BusCan.cpp"
    "Encoder/Encoder.cpp"
    "Digital/DigitalInput.cpp"
    "Digital/DigitalOutput.cpp"
    "Motor/Motor.cpp"
    "Motor/MotorBLDC.cpp"
    "Motor/MotorStepper.cpp"
    "Motor/MotorStepperParam.cpp"
    "Usb/UsbConsole.cpp"
    "Usb/UsbSerial.cpp"
    "FlashLoader/FlashLoader.cpp"
    "Led/Led.cpp"
    "RTClock/RTClock.cpp"
    "Analog/AnalogOutput.cpp"
    "Analog/AnalogInput.cpp"
    "CAN/CAN.cpp"
    "RS/RS.cpp"
)

set(COMPONENT_INCLUDE_DIRS
    "Bus"
    "Encoder"
    "Digital"
    "Analog"
    "Motor"
    "Usb"
    "FlashLoader"
    "Led"
    "RTClock"
    "CAN"
    "RS"
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