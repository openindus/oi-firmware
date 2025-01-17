cmake_minimum_required(VERSION 3.16.0)

if(DEFINED ENV{IDF_PATH})
    include($ENV{IDF_PATH}/tools/cmake/project.cmake)
    list(APPEND EXTRA_COMPONENT_DIRS components/api)
    
else()
    add_executable(oi-firmware main/main.cpp)

    target_sources(oi-firmware PRIVATE
        components/system/System.cpp
        components/api/Module/CorePlus/CorePlus.cpp
        components/api/ComBus/Controller/Controller.cpp
        components/api/ComBus/Controller/ControllerMaster.cpp
        components/middleware/linux_arm/Led.cpp
        components/middleware/linux_arm/Bus.cpp
        components/middleware/linux_arm/Board.cpp
    )

    target_include_directories(oi-firmware PRIVATE
        components/system
        components/api
        components/api/OSAL
        components/api/Module
        components/api/Module/CorePlus
        components/api/ComBus
        components/api/ComBus/Controller
        components/api/ComBus/Commands
        components/middleware
        components/middleware/linux_arm/include
    )

    target_compile_definitions(oi-firmware PRIVATE 
        LINUX_ARM
        OI_CORE_PLUS
        MODULE_MASTER
    )

endif()

project(OI-Firmware)