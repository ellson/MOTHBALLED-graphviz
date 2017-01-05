# Header checks
include(CheckIncludeFile)

check_include_file(unistd.h HAVE_UNISTD_H)

# Function checks
include(CheckFunctionExists)

check_function_exists( drand48     HAVE_DRAND48    )
check_function_exists( cbrt        HAVE_CBRT       )
check_function_exists( srand48     HAVE_SRAND48    )
check_function_exists( strcasecmp  HAVE_STRCASECMP )

# Write check results to config.h header
configure_file(config-cmake.h.in config.h)
