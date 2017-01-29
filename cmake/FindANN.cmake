find_path(ANN_INCLUDE_DIR ANN/ANN.h)
find_library(ANN_LIBRARY NAMES ANN ann libann)
find_file(ANN_RUNTIME_LIBRARY ANN.dll)

include(FindPackageHandleStandardArgs)
if (WIN32)
    find_package_handle_standard_args(ANN DEFAULT_MSG
                                      ANN_LIBRARY ANN_INCLUDE_DIR ANN_RUNTIME_LIBRARY)
else()
    find_package_handle_standard_args(ANN DEFAULT_MSG
                                      ANN_LIBRARY ANN_INCLUDE_DIR)
endif()

mark_as_advanced(ANN_INCLUDE_DIR ANN_LIBRARY ANN_RUNTIME_LIBRARY)

set(ANN_INCLUDE_DIRS ${ANN_INCLUDE_DIR})
set(ANN_LIBRARIES ${ANN_LIBRARY})
set(ANN_RUNTIME_LIBRARIES ${ANN_RUNTIME_LIBRARY})
