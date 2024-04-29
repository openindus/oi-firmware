set(COMPONENT_SRCS
    "System/System.cpp"
    "Module/ModuleStandalone.cpp"
    "Module/ModuleMaster.cpp"
    "Module/ModuleSlave.cpp"
    "Module/ModuleControl.cpp"
    "Module/ModuleCLI.cpp"
    "Module/ModuleMasterCLI.cpp"
    "Module/Discrete/DiscreteStandalone.cpp"
    "Module/Discrete/DiscreteControl.cpp"
    "Module/Discrete/DiscreteSlave.cpp"
    "Module/Discrete/DiscreteCLI.cpp"
    "Module/Stepper/StepperStandalone.cpp"
    "Module/Stepper/StepperControl.cpp"
    "Module/Stepper/StepperSlave.cpp"
    "Module/Stepper/StepperCLI.cpp"
    "Module/Stepper/StepperParamCLI.cpp"
    "Module/Core/CoreStandalone.cpp"
    "Module/Core/CoreMaster.cpp"
    "Module/Core/CoreCLI.cpp"
    "Module/Mixed/MixedStandalone.cpp"
    "Module/Mixed/MixedControl.cpp"
    "Module/Mixed/MixedSlave.cpp"
    "Module/Mixed/MixedCLI.cpp"
    "Module/Relay/RelayStandalone.cpp"
    "Module/Relay/RelayControl.cpp"
    "Module/Relay/RelaySlave.cpp"
    "Module/Brushless/BrushlessStandalone.cpp"
    "Module/Brushless/BrushlessSlave.cpp"
    "Module/Brushless/BrushlessControl.cpp"
    "Module/AnalogLs/AnalogLsStandalone.cpp"
    "Module/AnalogLs/AnalogLsControl.cpp"
    "Module/AnalogLs/AnalogLsSlave.cpp"

    "Middleware/Bus/BusIO.cpp"
    "Middleware/Bus/BusRS.cpp"
    "Middleware/Bus/BusCAN.cpp"
    "Middleware/Encoder/Encoder.cpp"
    "Middleware/Digital/DigitalInputs.cpp"
    "Middleware/Digital/DigitalOutputs.cpp"
    "Middleware/Motor/Motor.cpp"
    "Middleware/Motor/MotorBLDC.cpp"
    "Middleware/Motor/MotorStepper.cpp"
    "Middleware/Motor/MotorStepperParam.cpp"
    "Middleware/Usb/UsbConsole.cpp"
    "Middleware/Usb/UsbSerial.cpp"
    "Middleware/FlashLoader/FlashLoader.cpp"
    "Middleware/Led/Led.cpp"
    "Middleware/RTClock/RTClock.cpp"
    "Middleware/Analog/AnalogOutputs.cpp"
    "Middleware/Analog/AnalogInputsLV.cpp"
    "Middleware/Analog/AnalogInputsHV.cpp"
    "Middleware/CAN/CAN.cpp"
    "Middleware/RS/RS.cpp"
)

set(COMPONENT_INCLUDE_DIRS
    "."
    "System"
    "Module"
    "Module/Discrete"
    "Module/Stepper"
    "Module/Core"
    "Module/Mixed"
    "Module/Relay"
    "Module/Brushless"
    "Module/AnalogLs"

    "Middleware/Bus"
    "Middleware/Encoder"
    "Middleware/Digital"
    "Middleware/Analog"
    "Middleware/Motor"
    "Middleware/Usb"
    "Middleware/FlashLoader"
    "Middleware/Led"
    "Middleware/RTClock"
    "Middleware/CAN"
    "Middleware/RS"
)

set(COMPONENT_PRIV_REQUIRES
    "esp_adc_cal"
    "console"
    "nvs_flash"
    "app_update"
    "fatfs"
    "sdmmc"
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