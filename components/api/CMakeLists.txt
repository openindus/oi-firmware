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
    "Cmds/Digital/DigitalInputsCmd.cpp"
    "Cmds/Digital/DigitalOutputsCmd.cpp"
    "Cmds/Relay/RelayCmd.cpp"
    "Cmds/Stepper/StepperCmd.cpp"
    "Cmds/Dc/DcCmd.cpp"
    "Cmds/Analog/AnalogOutputsCmd.cpp"
    "Cmds/Analog/AnalogInputsLSCmd.cpp"
    "Cmds/Analog/AnalogInputsLVCmd.cpp"
    "Cmds/Analog/AnalogInputsHVCmd.cpp"
    "Cmds/Digital/DigitalInputsCmdHandler.cpp"
    "Cmds/Digital/DigitalOutputsCmdHandler.cpp"
    "Cmds/Relay/RelayCmdHandler.cpp"
    "Cmds/Stepper/StepperCmdHandler.cpp"
    "Cmds/Dc/DcCmdHandler.cpp"
    "Cmds/Analog/AnalogOutputsCmdHandler.cpp"
    "Cmds/Analog/AnalogInputsLSCmdHandler.cpp"
    "Cmds/Analog/AnalogInputsLVCmdHandler.cpp"
    "Cmds/Analog/AnalogInputsHVCmdHandler.cpp"
    "Cmds/Encoder/EncoderCmd.cpp"
    "Cmds/Encoder/EncoderCmdHandler.cpp"
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
    "Cmds"
    "Cmds/Analog"
    "Cmds/Digital"
    "Cmds/Relay"
    "Cmds/Stepper"
    "Cmds/Dc"
    "Cmds/Analog"
    "Cmds/Digital"
    "Cmds/Relay"
    "Cmds/Stepper"
    "Cmds/Dc"
    "Cmds/Encoder"
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