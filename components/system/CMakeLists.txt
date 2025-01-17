set(COMPONENT_SRCS
    "System.cpp"
)

set(COMPONENT_INCLUDE_DIRS
    "."
)

set(COMPONENT_PRIV_REQUIRES
    "esp_adc_cal"
    "console"
    "nvs_flash"
    "app_update"
    "fatfs"
    "sdmmc"
    "esp_rom"
)

set(COMPONENT_REQUIRES
    "middleware"
    "api"
)

idf_component_register(
    SRCS ${COMPONENT_SRCS}
    INCLUDE_DIRS ${COMPONENT_INCLUDE_DIRS}
    PRIV_REQUIRES ${COMPONENT_PRIV_REQUIRES}
    REQUIRES ${COMPONENT_REQUIRES}
)