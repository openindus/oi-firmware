cmake_minimum_required(VERSION 3.16.0)

if(DEFINED ENV{IDF_PATH})
    include($ENV{IDF_PATH}/tools/cmake/project.cmake)
    list(APPEND EXTRA_COMPONENT_DIRS external_components/espressif)
    
else()
    add_executable(oi-firmware main/main.cpp)

    target_sources(oi-firmware PRIVATE
        components/api/System/System.cpp
    )

    target_include_directories(oi-firmware PRIVATE
        components/api
        components/api/System
    )

    target_compile_definitions(oi-firmware PRIVATE 
        LINUX_ARM
        OI_CORE_PLUS
        MODULE_MASTER
    )

endif()

project(OI-Firmware)