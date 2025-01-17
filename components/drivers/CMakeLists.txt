set(COMPONENT_SRCS
    "esp32/dac8760/dac8760.c"
    "esp32/powerSTEP01/PS01_Cmd.c"
    "esp32/powerSTEP01/PS01_Hal.c"
    "esp32/powerSTEP01/PS01_Param.c"
    "esp32/powerSTEP01/PS01_Utils.c"
    "esp32/powerSTEP01/PS01.c"
    "esp32/pcal6524/pcal6524.c"
    "esp32/m41t62/m41t62.c"
    "esp32/m41t62/STM32DriversComponents/M41T62.c"
    "esp32/sc16is750/sc16is750.c"
    "esp32/mcp25625/mcp25625.c"
    "esp32/mcp25625/DriversComponents/MCP25625.c"
    "esp32/mcp25625/DriversComponents/MCP25625_hw.c"
    "esp32/mcp25625/DriversComponents/MCP25625_hal.c"
    "esp32/stds75/stds75.c"
    "esp32/cd74hc4052/cd74hc4052.c"
    "esp32/dac/ad5413/ad5413.c"
    "esp32/adc/ads114s0x/ads114s0x.c"
    "esp32/adc/ads866x/ads866x.c"
)

set(COMPONENT_INCLUDE_DIRS
    "esp32/."
    "esp32/dac8760"
    "esp32/powerSTEP01"
    "esp32/pcal6524"
    "esp32/m41t62"
    "esp32/sc16is750"
    "esp32/mcp25625"
    "esp32/stds75"
    "esp32/cd74hc4052"
    "esp32/dac"
    "esp32/dac/ad5413"
    "esp32/adc"
    "esp32/adc/ads866x"
    "esp32/adc/ads114s0x"
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
