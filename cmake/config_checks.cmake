# Header checks
include(CheckIncludeFile)

CHECK_INCLUDE_FILE(unistd.h HAVE_UNISTD_H)

# Function checks
include(CheckFunctionExists)

CHECK_FUNCTION_EXISTS(strcasecmp HAVE_STRCASECMP)

# Write check results to config.h header
configure_file(config-cmake.h.in config.h)
