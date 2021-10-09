set(COMPONENT_SRCS
    "ads866x/ads866x.c"
    "dac8760/dac8760.c"
    "ledonewire/ledonewire.c"
    "powerSTEP01/powerSTEP01.c"
    "powerSTEP01/STM32DriversComponents/powerstep01.c"
    "L6470/L6470.c"
    "L6470/CustomDriversComponents/l6470.c"
    "pcal6524/pcal6524.c"
    "m41t62/m41t62.c"
    "m41t62/STM32DriversComponents/M41T62.c"
    "sc16is750/sc16is750.c"
    "mcp25625/mcp25625.c"
    "mcp25625/DriversComponents/MCP25625.c"
    "mcp25625/DriversComponents/MCP25625_hw.c"
    "mcp25625/DriversComponents/MCP25625_hal.c"
)

set(COMPONENT_INCLUDE_DIRS
    "."
)

set(COMPONENT_PRIV_REQUIRES
    "nvs_flash"
)

idf_component_register(
    SRCS ${COMPONENT_SRCS}
    INCLUDE_DIRS ${COMPONENT_INCLUDE_DIRS}
    PRIV_REQUIRES ${COMPONENT_PRIV_REQUIRES}
)

set_source_files_properties(powerSTEP01/STM32DriversComponents/powerstep01.c
    PROPERTIES COMPILE_FLAGS
    "-Wno-missing-field-initializers -Wno-implicit-fallthrough -Wno-maybe-uninitialized"
)

set_source_files_properties(L6470/CustomDriversComponents/l6470.c
    PROPERTIES COMPILE_FLAGS
    "-Wno-missing-field-initializers -Wno-implicit-fallthrough -Wno-maybe-uninitialized"
)