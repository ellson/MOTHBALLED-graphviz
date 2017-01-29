if (with_smyrna)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${GRAPHVIZ_LIB_DIR}/common
    ${GRAPHVIZ_LIB_DIR}/neatogen
)

add_library(topfish STATIC
    # Header files
    hierarchy.h

    # Source files
    hierarchy.c
    rescale_layout.c
)

endif (with_smyrna)
