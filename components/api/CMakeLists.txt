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
    "Controller/MasterController.cpp"
    "Controller/MasterControllerCLI.cpp"
    "Controller/SlaveController.cpp"
    "Controller/SlaveControllerCLI.cpp"
    "Controller/Controller.cpp"
    "Commands/Digital/DigitalInputsCmd.cpp"
    "Commands/Digital/DigitalOutputsCmd.cpp"
    "Commands/Relay/RelayCmd.cpp"
    "Commands/Stepper/StepperCmd.cpp"
    "Commands/Dc/DcCmd.cpp"
    "Commands/Analog/AnalogOutputsCmd.cpp"
    "Commands/Analog/AnalogInputsLSCmd.cpp"
    "Commands/Analog/AnalogInputsLVCmd.cpp"
    "Commands/Analog/AnalogInputsHVCmd.cpp"
    "Commands/Digital/DigitalInputsCmdHandler.cpp"
    "Commands/Digital/DigitalOutputsCmdHandler.cpp"
    "Commands/Relay/RelayCmdHandler.cpp"
    "Commands/Stepper/StepperCmdHandler.cpp"
    "Commands/Dc/DcCmdHandler.cpp"
    "Commands/Analog/AnalogOutputsCmdHandler.cpp"
    "Commands/Analog/AnalogInputsLSCmdHandler.cpp"
    "Commands/Analog/AnalogInputsLVCmdHandler.cpp"
    "Commands/Analog/AnalogInputsHVCmdHandler.cpp"
    "Commands/Encoder/EncoderCmd.cpp"
    "Commands/Encoder/EncoderCmdHandler.cpp"
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
    "Commands"
    "Commands/Analog"
    "Commands/Digital"
    "Commands/Relay"
    "Commands/Stepper"
    "Commands/Dc"
    "Commands/Analog"
    "Commands/Digital"
    "Commands/Relay"
    "Commands/Stepper"
    "Commands/Dc"
    "Commands/Encoder"
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