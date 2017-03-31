find_path(GD_INCLUDE_DIR gd.h)
find_library(GD_LIBRARY NAMES gd libgd)
find_file(GD_RUNTIME_LIBRARY libgd.dll)

include(FindPackageHandleStandardArgs)
if (WIN32)
    find_package_handle_standard_args(GD DEFAULT_MSG
                                      GD_LIBRARY GD_INCLUDE_DIR GD_RUNTIME_LIBRARY)
else()
    find_package_handle_standard_args(GD DEFAULT_MSG
                                      GD_LIBRARY GD_INCLUDE_DIR)
endif()

mark_as_advanced(GD_INCLUDE_DIR GD_LIBRARY GD_RUNTIME_LIBRARY)

set(GD_INCLUDE_DIRS ${GD_INCLUDE_DIR})
set(GD_LIBRARIES ${GD_LIBRARY})
set(GD_RUNTIME_LIBRARIES ${GD_RUNTIME_LIBRARY})
