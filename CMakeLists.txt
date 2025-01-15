cmake_minimum_required(VERSION 3.16.0)

if(DEFINED ENV{IDF_PATH})
    include($ENV{IDF_PATH}/tools/cmake/project.cmake)
    list(APPEND EXTRA_COMPONENT_DIRS components/api)
    
else()
    add_executable(OI-Firmware main/main.cpp)

    target_sources(OI-Firmware PRIVATE
        components/system/System.cpp
    )

    target_include_directories(OI-Firmware PRIVATE
        components/system
        components/api
        components/api/Module
        components/api/Module/Core
        components/api/ComBus
        components/middleware
        components/middleware/linux_arm/include
    )

    target_compile_definitions(OI-Firmware PRIVATE 
        LINUX_ARM
        OI_CORE_PLUS
    )

endif()

project(OI-Firmware)