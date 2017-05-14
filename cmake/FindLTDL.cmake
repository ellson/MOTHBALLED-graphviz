find_path(LTDL_INCLUDE_DIR ltdl.h)
if (NOT WIN32)
    find_library(LTDL_LIBRARY NAMES libtldl ltdl)
endif()

include(FindPackageHandleStandardArgs)
if (NOT WIN32)
    find_package_handle_standard_args(LTDL DEFAULT_MSG
                                      LTDL_LIBRARY LTDL_INCLUDE_DIR)
else()
    find_package_handle_standard_args(LTDL DEFAULT_MSG
                                      LTDL_INCLUDE_DIR)
endif()

mark_as_advanced(LTDL_INCLUDE_DIR LTDL_LIBRARY)

set(LTDL_INCLUDE_DIRS ${LTDL_INCLUDE_DIR})
set(LTDL_LIBRARIES ${LTDL_LIBRARY})
