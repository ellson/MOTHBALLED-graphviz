if (NOT HAVE_GETOPT_H)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

add_library(getopt STATIC
    # Header files
    getopt.h

    # Source files
    getopt.c
)

endif (NOT HAVE_GETOPT_H)
