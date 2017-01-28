include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${GRAPHVIZ_LIB_DIR}/cdt
    ${GRAPHVIZ_LIB_DIR}/cgraph
)

add_library(ingraphs STATIC
    # Header files
    ingraphs.h

    # Source files
    ingraphs.c
)
