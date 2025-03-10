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
    "Controller/Controller.cpp"
    "Controller/DigitalCmd/DigitalInputsCmd.cpp"
    "Controller/DigitalCmd/DigitalOutputsCmd.cpp"
    "Controller/DigitalCmd/DigitalInputsCmdHandler.cpp"
    "Controller/DigitalCmd/DigitalOutputsCmdHandler.cpp"
    "Controller/RelayCmd/RelayCmd.cpp"
    "Controller/RelayCmd/RelayCmdHandler.cpp"
    "Controller/AnalogCmd/AnalogOutputsCmd.cpp"
    "Controller/AnalogCmd/AnalogInputsLSCmd.cpp"
    "Controller/AnalogCmd/AnalogInputsLVCmd.cpp"
    "Controller/AnalogCmd/AnalogInputsHVCmd.cpp"
    "Controller/AnalogCmd/AnalogOutputsCmdHandler.cpp"
    "Controller/AnalogCmd/AnalogInputsLSCmdHandler.cpp"
    "Controller/AnalogCmd/AnalogInputsLVCmdHandler.cpp"
    "Controller/AnalogCmd/AnalogInputsHVCmdHandler.cpp"
    "Controller/MotorCmd/MotorStepperCmd.cpp"
    "Controller/MotorCmd/MotorStepperCmdHandler.cpp"
    "Controller/DcCmd/DcCmd.cpp"
    "Controller/DcCmd/DcCmdHandler.cpp"
    "Controller/EncoderCmd/EncoderCmd.cpp"
    "Controller/EncoderCmd/EncoderCmdHandler.cpp"
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
    "Controller"
    "Controller/AnalogCmd"
    "Controller/DigitalCmd"
    "Controller/RelayCmd"
    "Controller/DcCmd"
    "Controller/AnalogCmd"
    "Controller/DigitalCmd"
    "Controller/RelayCmd"
    "Controller/MotorCmd"
    "Controller/DcCmd"
    "Controller/EncoderCmd"
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