set(COMPONENT_SRCS
    "src/Ethernet.c"
    "src/Event.c"
    "src/LTE.c"
    "src/Mac.c"
    "src/Wifi.c"
)

set(COMPONENT_INCLUDE_DIRS
    "include"
)

set(COMPONENT_REQUIRES
    "esp_modem"
)

idf_component_register(
    SRCS ${COMPONENT_SRCS}
    INCLUDE_DIRS ${COMPONENT_INCLUDE_DIRS}
    REQUIRES ${COMPONENT_REQUIRES}
)