find_path(LTDL_INCLUDE_DIR ltdl.h)
find_library(LTDL_LIBRARY NAMES libtldl ltdl)
find_file(LTDL_RUNTIME_LIBRARY libltdl3.dll)

include(FindPackageHandleStandardArgs)
if (WIN32)
    find_package_handle_standard_args(LTDL DEFAULT_MSG
                                      LTDL_LIBRARY LTDL_INCLUDE_DIR LTDL_RUNTIME_LIBRARY)
else()
    find_package_handle_standard_args(LTDL DEFAULT_MSG
                                      LTDL_LIBRARY LTDL_INCLUDE_DIR)
endif()

mark_as_advanced(LTDL_INCLUDE_DIR LTDL_LIBRARY LTDL_RUNTIME_LIBRARY)

set(LTDL_INCLUDE_DIRS ${LTDL_INCLUDE_DIR})
set(LTDL_LIBRARIES ${LTDL_LIBRARY})
set(LTDL_RUNTIME_LIBRARIES ${LTDL_RUNTIME_LIBRARY})
