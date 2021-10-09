set(COMPONENT_SRCS
    "OIMain.cpp"
    "OISystem.cpp"
    "OIConsole.cpp"
    "OIFunction.cpp"
    "OIBus.cpp"
    "OIModule.cpp"
    "OIType.cpp"
    "OIFlash.cpp"
    "OICore.cpp"
    "OIDiscrete.cpp"
    "OIStepper.cpp"
    "OIMixed.cpp"
    "OIRelay.cpp"
    "OIRS.cpp"
    "OICAN.cpp"
    "OIWire.cpp"
)

set(COMPONENT_INCLUDE_DIRS
    "."
)

set(COMPONENT_PRIV_REQUIRES
    "esp_adc_cal"
    "nvs_flash"
    "console"
    "app_update"
    "fatfs"
    "sdmmc"
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