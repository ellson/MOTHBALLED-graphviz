# Header checks
include(CheckIncludeFile)

check_include_file( crt_externs.h       HAVE_CRT_EXTERNS_H      )
check_include_file( fcntl .h            HAVE_FCNTL_H            )
check_include_file( malloc.h            HAVE_MALLOC_H           )
check_include_file( search.h            HAVE_SEARCH_H           )
check_include_file( stat.h              HAVE_STAT_H             )
check_include_file( strings.h           HAVE_STRINGS_H          )
check_include_file( sys/inotify.h       HAVE_SYS_INOTIFY_H      )
check_include_file( sys/ioctl.h         HAVE_SYS_IOCTL_H        )
check_include_file( sys/mman.h          HAVE_SYS_MMAN_H         )
check_include_file( sys/select.h        HAVE_SYS_SELECT_H       )
check_include_file( sys/stat.h          HAVE_SYS_STAT_H         )
check_include_file( sys/time.h          HAVE_SYS_TIME_H         )
check_include_file( sys/types.h         HAVE_SYS_TYPES_H        )
check_include_file( sys/vfork.h         HAVE_SYS_VFORK_H        )
check_include_file( termios.h           HAVE_TERMIOS_H          )
check_include_file( unistd.h            HAVE_UNISTD_H           )
check_include_file( vfork.h             HAVE_VFORK_H            )
check_include_file( X11/Intrinsic.h     HAVE_X11_INTRINSIC_H    )
check_include_file( X11/Xaw/Text.h      HAVE_X11_XAW_TEXT_H     )

# Function checks
include(CheckFunctionExists)

check_function_exists( _NSGetEnviron    HAVE__NSGETENVIRON  )
check_function_exists( drand48          HAVE_DRAND48        )
check_function_exists( cbrt             HAVE_CBRT           )
check_function_exists( getpagesize      HAVE_GETPAGESIZE    )
check_function_exists( getenv           HAVE_GETENV         )
check_function_exists( lrand48          HAVE_LRAND48        )
check_function_exists( mallinfo         HAVE_MALLINFO       )
check_function_exists( mallopt          HAVE_MALLOPT        )
check_function_exists( mstats           HAVE_MSTATS         )
check_function_exists( setenv           HAVE_SETENV         )
check_function_exists( setmode          HAVE_SETMODE        )
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
set( HAVE_LIBGD     ${GD_FOUND}         )
set( HAVE_ZLIB      ${ZLIB_FOUND}       )

if (LTDL_FOUND)
    set(ENABLE_LTDL 1)
    set(LTDL_H 1)
endif()

# Values
if (WIN32)

    set( BROWSER            start                                   )
    set( DEFAULT_FONTPATH   "C:/Windows/Fonts"                      )

elseif (APPLE)

    set( BROWSER            open                                    )
    set( DEFAULT_FONTPATH   "~/Library/Fonts:/Library/Fonts:"
                            "/Network/Library/Fonts:"
                            "/System/Library/Fonts"                 )
    set( DARWIN             1                                       )
    set( DARWIN_DYLIB       1                                       )

else ()

    set( BROWSER            xdg-open                                )
    set( DEFAULT_FONTPATH   "/usr/share/fonts/sun/TrueType:"
                            "/usr/share/fonts/TrueType:"
                            "/usr/X11/lib/X11/fonts/TrueType:"
                            "/usr/share/fonts/sun/Type1:"
                            "/usr/openwin/lib/X11/fonts/Type1:"
                            "/usr/share/fonts/X11/Type1:"
                            "/usr/share/fonts/default/Type1:"
                            "/usr/X11R6/lib/X11/fonts/TrueType:"
                            "/usr/X11R6/lib/X11/fonts/truetype:"
                            "/usr/X11R6/lib/X11/fonts/TTF:"
                            "/usr/share/fonts/truetype:"
                            "/usr/openwin/lib/X11/fonts/TrueType:"
                            "/usr/X11R6/lib/X11/fonts/Type1:"
                            "/usr/common/share/fonts/ttf"           )

endif ()

set(DEFAULT_DPI 96)

# Write check results to config.h header
configure_file(config-cmake.h.in config.h)
