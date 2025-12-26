idf_component_register(SRCS "main.c"
                    INCLUDE_DIRS ".")

file(GLOB_RECURSE EXTRA_COMPONENT_DIRS "${CMAKE_SOURCE_DIR}/components/*") 