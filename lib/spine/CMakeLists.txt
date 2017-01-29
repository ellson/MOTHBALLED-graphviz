include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${GRAPHVIZ_LIB_DIR}/cdt
    ${GRAPHVIZ_LIB_DIR}/cgraph
)

add_library(spine STATIC
    # Header files
    quad.h
    spine.h
    spinehdr.h
    subset.h
    union_find.h

    # Source files
    quad.c
    spine.c
    subset.c
    union_find.c
)
