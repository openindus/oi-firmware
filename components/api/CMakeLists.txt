set(COMPONENT_SRCS
    "System/System.cpp"
    "System/Master/Master.cpp"
    "System/Master/MasterCLI.cpp"
    "System/Slave/Slave.cpp"
    "System/Slave/SlaveCLI.cpp"

    "Module/Module.cpp"
    "Module/ModuleControl.cpp"
    "Module/Discrete/Discrete.cpp"
    "Module/Stepper/Stepper.cpp"
    "Module/RelayHP/RelayHP.cpp"
    "Module/Core/Core.cpp"
    "Module/Mixed/Mixed.cpp"
    "Module/AnalogLS/AnalogLS.cpp"
    "Module/Dc/Dc.cpp"

    "Middleware/Digital/Inputs/DigitalInputs.cpp"
    "Middleware/Digital/Inputs/DigitalInputsCLI.cpp"
    "Middleware/Digital/Inputs/DigitalInputsCmdHandler.cpp"
    "Middleware/Digital/Inputs/DigitalInputsCmd.cpp"
    "Middleware/Digital/Outputs/DigitalOutputs.cpp"
    "Middleware/Digital/Outputs/DigitalOutputsCLI.cpp"
    "Middleware/Digital/Outputs/DigitalOutputsCmd.cpp"
    "Middleware/Digital/Outputs/DigitalOutputsCmdHandler.cpp"

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
    "System/Master"
    "System/Slave"

    "Module"
    "Module/Discrete"
    "Module/Stepper"
    "Module/Core"
    "Module/Mixed"
    "Module/RelayHP"
    "Module/Brushless"
    "Module/AnalogLS"
    "Module/Dc"

    "Middleware/Digital"
    "Middleware/Digital/Inputs"
    "Middleware/Digital/Outputs"

    "Cmd"
    "Cmd/Analog"
    "Cmd/Relay"
    "Cmd/Dc"
    "Cmd/Analog"
    "Cmd/Relay"
    "Cmd/Motor"
    "Cmd/Dc"
    "Cmd/Encoder"
)

set(COMPONENT_PRIV_REQUIRES

)

set(COMPONENT_REQUIRES
    "middleware"
    "drivers"
)

idf_component_register(
    SRCS ${COMPONENT_SRCS}
    INCLUDE_DIRS ${COMPONENT_INCLUDE_DIRS}
    PRIV_REQUIRES ${COMPONENT_PRIV_REQUIRES}
    REQUIRES ${COMPONENT_REQUIRES}
)

# component_compile_options(-std=c++17 -std=gnu++17)