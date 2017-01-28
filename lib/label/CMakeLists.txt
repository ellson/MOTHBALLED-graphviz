include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${GRAPHVIZ_LIB_DIR}/cdt
    ${GRAPHVIZ_LIB_DIR}/cgraph
    ${GRAPHVIZ_LIB_DIR}/common
)

add_library(label STATIC
    # Header files
    index.h
    node.h
    rectangle.h
    split.q.h
    xlabels.h

    # Source files
    index.c
    node.c
    rectangle.c
    split.q.c
    xlabels.c
)
