set(COMPONENT_SRCDIRS
    "."
)

set(COMPONENT_ADD_INCLUDEDIRS
    "."
)

set(COMPONENT_REQUIRES
    "Arduino"
)

register_component()

target_compile_definitions(${COMPONENT_TARGET} PUBLIC -DESP32)
target_compile_options(${COMPONENT_TARGET} PRIVATE -fno-rtti)