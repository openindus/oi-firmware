set(srcs
    "src"
)

set(inc_dirs
    "include"
)

idf_component_register(SRCS "${srcs}"
                    INCLUDE_DIRS "${inc_dirs}"
                    PRIV_INCLUDE_DIRS ".")