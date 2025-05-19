set(COMPONENT_SRCS
    "System/System.cpp"
    "Module/Module.cpp"
    "Module/Discrete/Discrete.cpp"
    "Module/Stepper/Stepper.cpp"
    "Module/RelayHP/RelayHP.cpp"
    "Module/Core/Core.cpp"
    "Module/Mixed/Mixed.cpp"
    "Module/AnalogLS/AnalogLS.cpp"
    "Module/Dc/Dc.cpp"
    "Master/Master.cpp"
    "Master/MasterCLI.cpp"
    "Slave/Slave.cpp"
    "Slave/SlaveCLI.cpp"
    "Module/ModuleControl.cpp"
    "Cmd/Digital/DigitalInputsCmd.cpp"
    "Cmd/Digital/DigitalOutputsCmd.cpp"
    "Cmd/Digital/DigitalInputsCmdHandler.cpp"
    "Cmd/Digital/DigitalOutputsCmdHandler.cpp"
    "Cmd/Relay/RelayCmd.cpp"
    "Cmd/Relay/RelayCmdHandler.cpp"
    "Cmd/Analog/AnalogOutputsCmd.cpp"
    "Cmd/Analog/AnalogInputsLSCmd.cpp"
    "Cmd/Analog/AnalogInputsLVCmd.cpp"
    "Cmd/Analog/AnalogInputsHVCmd.cpp"
    "Cmd/Analog/AnalogOutputsCmdHandler.cpp"
    "Cmd/Analog/AnalogInputsLSCmdHandler.cpp"
    "Cmd/Analog/AnalogInputsLVCmdHandler.cpp"
    "Cmd/Analog/AnalogInputsHVCmdHandler.cpp"
    "Cmd/Motor/MotorStepperCmd.cpp"
    "Cmd/Motor/MotorStepperCmdHandler.cpp"
    "Cmd/Motor/MotorStepperCmdCLI.cpp"
    "Cmd/Dc/DcCmd.cpp"
    "Cmd/Dc/DcCmdHandler.cpp"
    "Cmd/Encoder/EncoderCmd.cpp"
    "Cmd/Encoder/EncoderCmdHandler.cpp"
)

set(COMPONENT_INCLUDE_DIRS
    "."
    "System"
    "Module"
    "Module/Discrete"
    "Module/Stepper"
    "Module/Core"
    "Module/Mixed"
    "Module/RelayHP"
    "Module/Brushless"
    "Module/AnalogLS"
    "Module/Dc"
    "Master"
    "Slave"
    "Cmd"
    "Cmd/Analog"
    "Cmd/Digital"
    "Cmd/Relay"
    "Cmd/Dc"
    "Cmd/Analog"
    "Cmd/Digital"
    "Cmd/Relay"
    "Cmd/Motor"
    "Cmd/Dc"
    "Cmd/Encoder"
)

set(COMPONENT_PRIV_REQUIRES

)

set(COMPONENT_REQUIRES
    "middleware"
)

idf_component_register(
    SRCS ${COMPONENT_SRCS}
    INCLUDE_DIRS ${COMPONENT_INCLUDE_DIRS}
    PRIV_REQUIRES ${COMPONENT_PRIV_REQUIRES}
    REQUIRES ${COMPONENT_REQUIRES}
)

# component_compile_options(-std=c++17 -std=gnu++17)