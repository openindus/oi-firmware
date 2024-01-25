set(COMPONENT_SRCS
    "ads866x/ads866x.c"
    "dac8760/dac8760.c"
    "ledonewire/ledonewire.c"
    "powerSTEP01/PS01_Cmd.c"
    "powerSTEP01/PS01_Hal.c"
    "powerSTEP01/PS01_Param.c"
    "powerSTEP01/PS01_Utils.c"
    "powerSTEP01/PS01.c"
    "pcal6524/pcal6524.c"
    "m41t62/m41t62.c"
    "m41t62/STM32DriversComponents/M41T62.c"
    "sc16is750/sc16is750.c"
    "mcp25625/mcp25625.c"
    "mcp25625/DriversComponents/MCP25625.c"
    "mcp25625/DriversComponents/MCP25625_hw.c"
    "mcp25625/DriversComponents/MCP25625_hal.c"
    "ads1x4s08/ads1x4s08.c"
    "stds75/stds75.c"
    "cd74hc4052/cd74hc4052.c"
)

set(COMPONENT_INCLUDE_DIRS
    "."
    "ledonewire"
    "ads866x"
    "dac8760"
    "powerSTEP01"
    "pcal6524"
    "m41t62"
    "sc16is750"
    "mcp25625"
    "ads1x4s08"
    "stds75"
    "cd74hc4052"
)

set(COMPONENT_PRIV_REQUIRES
    ""
)

set(COMPONENT_REQUIRES
    "nvs_flash"
)

idf_component_register(
    SRCS ${COMPONENT_SRCS}
    INCLUDE_DIRS ${COMPONENT_INCLUDE_DIRS}
    PRIV_REQUIRES ${COMPONENT_PRIV_REQUIRES}
    REQUIRES ${COMPONENT_REQUIRES}
)

# set_source_files_properties(powerSTEP01/powerSTEP01.c
#     PROPERTIES COMPILE_FLAGS
#     "-Wno-missing-field-initializers -Wno-implicit-fallthrough -Wno-maybe-uninitialized"
# )
