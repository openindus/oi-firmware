set(COMPONENT_SRCS
    "esp32/Bus/BusIO.cpp"
    "esp32/Bus/BusRS.cpp"
    "esp32/Bus/BusCAN.cpp"
    "esp32/Encoder/Encoder.cpp"
    "esp32/Digital/Inputs/DigitalInputs.cpp"
    "esp32/Digital/Inputs/DigitalInputsExp.cpp"
    "esp32/Digital/Outputs/DigitalOutputs.cpp"
    "esp32/Digital/Outputs/DigitalOutputsExp.cpp"
    "esp32/Relays/Relays.cpp"
    "esp32/Motor/Motor.cpp"
    "esp32/Motor/Brushless/MotorBLDC.cpp"
    "esp32/Motor/Stepper/MotorStepper.cpp"
    "esp32/Motor/Stepper/MotorStepperParam.cpp"
    "esp32/Motor/Dc/MotorDc.cpp"
    "esp32/Usb/UsbConsole.cpp"
    "esp32/Usb/UsbSerial.cpp"
    "esp32/FlashLoader/FlashLoader.cpp"
    "esp32/Led/Led.cpp"
    "esp32/RTClock/RTClock.cpp"
    "esp32/Analog/Outputs/AnalogOutputs.cpp"
    "esp32/Analog/InputsLV/AnalogInputsLV.cpp"
    "esp32/Analog/InputsHV/AnalogInputsHV.cpp"
    "esp32/Analog/InputsLS/AnalogInputsLS.cpp"
    "esp32/Analog/InputsLS/ADC/ADS114S0X/_ADS114S0X.cpp"
    "esp32/Analog/InputsLS/Multiplexer/Multiplexer.cpp"
    "esp32/Analog/InputsLS/Digipot/Digipot.cpp"
    "esp32/Analog/InputsLS/Sensors/global_sensor.cpp"
    "esp32/Analog/InputsLS/Sensors/RawSensor/RawSensor.cpp"
    "esp32/Analog/InputsLS/Sensors/RTD/RTD.cpp"
    "esp32/Analog/InputsLS/Sensors/Thermocouple/Thermocouple.cpp"
    "esp32/Analog/InputsLS/Sensors/StrainGauge/StrainGauge.cpp"
    "esp32/CAN/CAN.cpp"
    "esp32/RS/RS.cpp"
    "esp32/Board/Board.cpp"
)

set(COMPONENT_INCLUDE_DIRS
    "."
    "esp32/Bus"
    "esp32/Encoder"
    "esp32/Digital/Inputs"
    "esp32/Digital/Outputs"
    "esp32/Relays"
    "esp32/Analog"
    "esp32/Analog/InputsHV"
    "esp32/Analog/InputsLV"
    "esp32/Analog/InputsLS"
    "esp32/Analog/InputsLS/ADC"
    "esp32/Analog/InputsLS/ADC/ADS114S0X"
    "esp32/Analog/InputsLS/Multiplexer"
    "esp32/Analog/InputsLS/Digipot"
    "esp32/Analog/InputsLS/Sensors"
    "esp32/Analog/InputsLS/Sensors/RawSensor"
    "esp32/Analog/InputsLS/Sensors/RTD"
    "esp32/Analog/InputsLS/Sensors/Thermocouple"
    "esp32/Analog/InputsLS/Sensors/StrainGauge"
    "esp32/Analog/Outputs"
    "esp32/Motor"
    "esp32/Motor/Brushless"
    "esp32/Motor/Stepper"
    "esp32/Motor/Dc"
    "esp32/Usb"
    "esp32/FlashLoader"
    "esp32/Led"
    "esp32/RTClock"
    "esp32/CAN"
    "esp32/RS"
    "esp32/Board"
)

set(COMPONENT_PRIV_REQUIRES

)

set(COMPONENT_REQUIRES
    "drivers"
    "esp_adc_cal"
    "console"
    "nvs_flash"
    "app_update"
    "fatfs"
    "sdmmc"
    "esp_rom"
)

idf_component_register(
    SRCS ${COMPONENT_SRCS}
    INCLUDE_DIRS ${COMPONENT_INCLUDE_DIRS}
    PRIV_REQUIRES ${COMPONENT_PRIV_REQUIRES}
    REQUIRES ${COMPONENT_REQUIRES}
)