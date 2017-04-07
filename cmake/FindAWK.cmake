find_program(AWK_EXECUTABLE NAMES gawk awk)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(AWK DEFAULT_MSG
                                  AWK_EXECUTABLE)
