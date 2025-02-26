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
    "Controller/Master/MasterController.cpp"
    "Controller/Master/MasterControllerCLI.cpp"
    "Controller/Slave/SlaveController.cpp"
    "Controller/Slave/SlaveControllerCLI.cpp"
    "Controller/Controller.cpp"
    "Controller/Cmd/Digital/DigitalInputsCmd.cpp"
    "Controller/Cmd/Digital/DigitalOutputsCmd.cpp"
    "Controller/Cmd/Relay/RelayCmd.cpp"
    "Controller/Cmd/Stepper/StepperCmd.cpp"
    "Controller/Cmd/Dc/DcCmd.cpp"
    "Controller/Cmd/Analog/AnalogOutputsCmd.cpp"
    "Controller/Cmd/Analog/AnalogInputsLSCmd.cpp"
    "Controller/Cmd/Analog/AnalogInputsLVCmd.cpp"
    "Controller/Cmd/Analog/AnalogInputsHVCmd.cpp"
    "Controller/Cmd/Digital/DigitalInputsCmdHandler.cpp"
    "Controller/Cmd/Digital/DigitalOutputsCmdHandler.cpp"
    "Controller/Cmd/Relay/RelayCmdHandler.cpp"
    "Controller/Cmd/Stepper/StepperCmdHandler.cpp"
    "Controller/Cmd/Dc/DcCmdHandler.cpp"
    "Controller/Cmd/Analog/AnalogOutputsCmdHandler.cpp"
    "Controller/Cmd/Analog/AnalogInputsLSCmdHandler.cpp"
    "Controller/Cmd/Analog/AnalogInputsLVCmdHandler.cpp"
    "Controller/Cmd/Analog/AnalogInputsHVCmdHandler.cpp"
    "Controller/Cmd/Encoder/EncoderCmd.cpp"
    "Controller/Cmd/Encoder/EncoderCmdHandler.cpp"
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
    "Controller"
    "Controller/Master"
    "Controller/Slave"
    "Controller/Cmd"
    "Controller/Cmd/Analog"
    "Controller/Cmd/Digital"
    "Controller/Cmd/Relay"
    "Controller/Cmd/Stepper"
    "Controller/Cmd/Dc"
    "Controller/Cmd/Analog"
    "Controller/Cmd/Digital"
    "Controller/Cmd/Relay"
    "Controller/Cmd/Stepper"
    "Controller/Cmd/Dc"
    "Controller/Cmd/Encoder"
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