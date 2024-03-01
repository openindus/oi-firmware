set(COMPONENT_SRCS
    "System/System.cpp"
    "Module/Discrete/DiscreteStandalone.cpp"
    "Module/Discrete/DiscreteControl.cpp"
    "Module/Discrete/DiscreteSlave.cpp"
    "Module/Stepper/StepperStandalone.cpp"
    "Module/Stepper/StepperControl.cpp"
    "Module/Stepper/StepperSlave.cpp"
    "Module/Core/CoreStandalone.cpp"
    "Module/Core/CoreMaster.cpp"
    "Module/Mixed/MixedStandalone.cpp"
    "Module/Mixed/MixedControl.cpp"
    "Module/Mixed/MixedSlave.cpp"
    "Module/Relay/RelayStandalone.cpp"
    "Module/Relay/RelayControl.cpp"
    "Module/Relay/RelaySlave.cpp"
    "Module/ModuleStandalone.cpp"
    "Module/ModuleMaster.cpp"
    "Module/ModuleSlave.cpp"
    "Module/ModuleControl.cpp"
    "Console/ConsoleModule.cpp"
    "Console/ConsoleMaster.cpp"
    "Console/ConsoleCore.cpp"
    "Console/ConsoleDiscrete.cpp"
    "Console/ConsoleMixed.cpp"
    "Console/ConsoleStepper.cpp"
    "Console/MotorStepperParamCLI.cpp"
    "Module/Brushless/BrushlessStandalone.cpp"
    "Module/Brushless/BrushlessSlave.cpp"
    "Module/Brushless/BrushlessControl.cpp"
    "Module/AnalogLS/AnaloglsStandalone.cpp"
    "Module/AnalogLS/AnaloglsControl.cpp"
    "Module/AnalogLS/AnaloglsSlave.cpp"
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
    "Module/AnalogLS"
    "Console"
)

set(COMPONENT_PRIV_REQUIRES
    "esp_adc_cal"
    "console"
    "nvs_flash"
    "app_update"
    "fatfs"
    "sdmmc"
)

set(COMPONENT_REQUIRES
    "drivers"
    "middleware"
)

idf_component_register(
    SRCS ${COMPONENT_SRCS}
    INCLUDE_DIRS ${COMPONENT_INCLUDE_DIRS}
    PRIV_REQUIRES ${COMPONENT_PRIV_REQUIRES}
    REQUIRES ${COMPONENT_REQUIRES}
)