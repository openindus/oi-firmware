cmake_minimum_required(VERSION 3.16.0)

if(DEFINED ENV{IDF_PATH})
    include($ENV{IDF_PATH}/tools/cmake/project.cmake)
    list(APPEND EXTRA_COMPONENT_DIRS components/drivers)
    list(APPEND EXTRA_COMPONENT_DIRS components/middleware)
    list(APPEND EXTRA_COMPONENT_DIRS components/api)
    list(APPEND EXTRA_COMPONENT_DIRS external_components/espressif/esp_modem)
    
else()
    add_executable(oi-firmware main/main.cpp)

    target_sources(oi-firmware PRIVATE
        components/api/System/System.cpp
        components/api/Module/CorePlus/CorePlus.cpp
        components/api/Controller/Controller.cpp
        components/api/Controller/Master/MasterController.cpp
        components/middleware/_linux_arm/Led_linux.cpp
        components/middleware/_linux_arm/Bus_linux.cpp
        components/middleware/_linux_arm/Board_linux.cpp
    )

    target_include_directories(oi-firmware PRIVATE
        components/system
        components/api
        components/api/Module
        components/api/Module/CorePlus
        components/api/Controller
        components/api/Controller/Cmd
        components/middleware
        components/middleware/Board
        components/middleware/Bus
        components/middleware/Led
    )

    target_compile_definitions(oi-firmware PRIVATE 
        LINUX_ARM
        OI_CORE_PLUS
        MODULE_MASTER
    )

endif()

project(OI-Firmware)