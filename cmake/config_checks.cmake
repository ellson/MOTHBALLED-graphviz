# Header checks
include(CheckIncludeFile)

check_include_file( malloc.h        HAVE_MALLOC_H       )
check_include_file( stat.h          HAVE_STAT_H         )
check_include_file( strings.h       HAVE_STRINGS_H      )
check_include_file( sys/stat.h      HAVE_SYS_STAT_H     )
check_include_file( sys/types.h     HAVE_SYS_TYPES_H    )
check_include_file( unistd.h        HAVE_UNISTD_H       )

# Function checks
include(CheckFunctionExists)

check_function_exists( drand48          HAVE_DRAND48        )
check_function_exists( cbrt             HAVE_CBRT           )
check_function_exists( getpagesize      HAVE_GETPAGESIZE    )
check_function_exists( mallinfo         HAVE_MALLINFO       )
check_function_exists( mallopt          HAVE_MALLOPT        )
check_function_exists( mstats           HAVE_MSTATS         )
check_function_exists( sincos           HAVE_SINCOS         )
check_function_exists( srand48          HAVE_SRAND48        )
check_function_exists( strcasecmp       HAVE_STRCASECMP     )
check_function_exists( strncasecmp      HAVE_STRNCASECMP    )
check_function_exists( vsnprintf        HAVE_VSNPRINTF      )

# Type checks
# The function check_size_type also checks if the type exists
# and sets HAVE_${VARIABLE} accordingly.
include(CheckTypeSize)

check_type_size( ssize_t     SSIZE_T     )
check_type_size( intptr_t    INTPTR_T    )

# Library checks
set( HAVE_ANN       ${ANN_FOUND}        )
set( HAVE_EXPAT     ${EXPAT_FOUND}      )
set( HAVE_ZLIB      ${ZLIB_FOUND}       )

if (LTDL_FOUND)
    set(ENABLE_LTDL 1)
    set(LTDL_H 1)
endif()

# Values
set(DEFAULT_DPI 96)

# Write check results to config.h header
configure_file(config-cmake.h.in config.h)
