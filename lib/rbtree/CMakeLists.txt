include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
)

add_library(rbtree STATIC
    # Header files
    misc.h
    red_black_tree.h
    stack.h

    # Source files
    misc.c
    red_black_tree.c
    stack.c
)
