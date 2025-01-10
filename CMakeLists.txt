cmake_minimum_required(VERSION 3.16.0)

if(DEFINED ENV{IDF_PATH})
    include($ENV{IDF_PATH}/tools/cmake/project.cmake)
    list(APPEND EXTRA_COMPONENT_DIRS components/api)
    
else()
    add_executable(OI-Firmware main/main.cpp)

    target_sources(OI-Firmware PRIVATE
        components/api/System/System.cpp
    )

    target_include_directories(OI-Firmware PRIVATE
        components/api
        components/api/System
    )
endif()

project(OI-Firmware)