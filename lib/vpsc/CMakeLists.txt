if (with_ipsepcola)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
)

add_library(vpsc STATIC
    # Header files
    block.h
    blocks.h
    constraint.h
    csolve_VPSC.h
    generate-constraints.h
    pairingheap/PairingHeap.h
    pairingheap/dsexceptions.h
    remove_rectangle_overlap.h
    solve_VPSC.h
    variable.h

    # Source files
    block.cpp
    blocks.cpp
    constraint.cpp
    csolve_VPSC.cpp
    generate-constraints.cpp
    pairingheap/PairingHeap.cpp
    remove_rectangle_overlap.cpp
    solve_VPSC.cpp
    variable.cpp
)

endif (with_ipsepcola)