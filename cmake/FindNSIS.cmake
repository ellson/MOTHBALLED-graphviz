# CMake does not allow for braces in $ENV{}, so a temporary variable must be used.
set(PROGRAMFILES_X86 "ProgramFiles(x86)")

find_program(NSIS_MAKE
    NAMES makensis
    PATHS $ENV{PROGRAMFILES}/NSIS $ENV{${PROGRAMFILES_X86}}/NSIS
)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(NSIS DEFAULT_MSG NSIS_MAKE)
