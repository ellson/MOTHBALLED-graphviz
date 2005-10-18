# Process this file with autoconf to produce a configure script.
# Configure template for gd library

#
AC_PREREQ(2.54)

#HEY! Change BOTH the vesion number and the GDLIB_REVISION setting!
AC_INIT([GD], [2.0.33], [gd@boutell.com])
AC_CONFIG_SRCDIR([gd.c])
AC_CONFIG_AUX_DIR(config)

GDLIB_MAJOR=2
GDLIB_MINOR=0
GDLIB_REVISION=33
GDLIBNAME=gd
#Expanded by tests later in this file. TBB 2.0.26
#2.0.28: GIF is standard now. Doesn't depend on anything else,
#so we always build it.
FEATURES=GD_GIF
AC_SUBST(FEATURES)

AC_SUBST(GDLIB_MAJOR)
AC_SUBST(GDLIB_MINOR)
AC_SUBST(GDLIB_REVISION)
AC_SUBST(GDLIBNAME)

AM_INIT_AUTOMAKE()
AC_CONFIG_HEADERS([config.h:config.hin])

AC_PROG_CC
AM_PROG_CC_STDC
AC_PROG_INSTALL
AC_PROG_LIBTOOL
AC_PROG_LN_S
AC_PROG_MAKE_SET
AC_PROG_RANLIB

dnl may be required for freetype and Xpm
AC_PATH_X

if test -n "$x_includes" && test "x$x_includes" != xNONE ; then
  CPPFLAGS="$CPPFLAGS -I$x_includes"
fi
if test -n "$x_libraries" && test "x$x_libraries" != xNONE ; then
  LDFLAGS="$LDFLAGS -L$x_libraries"
fi

AC_HEADER_STDC
AC_CHECK_HEADERS([errno.h limits.h stddef.h stdlib.h string.h unistd.h])

AM_ICONV
if test -n "$LIBICONV" ; then
  LIBS="$LIBS $LIBICONV"
fi

AC_CHECK_HEADERS(iconv.h,
  [AC_MSG_CHECKING(whether iconv.h defines iconv_t)
   AC_EGREP_HEADER([typedef.*iconv_t],iconv.h,
     [AC_MSG_RESULT(yes)
      AC_DEFINE(HAVE_ICONV_T_DEF, 1,
                [Define if <iconv.h> defines iconv_t.])],
      AC_MSG_RESULT(no))])

# Checks for typedefs, structures, and compiler characteristics.
#AC_C_CONST
#AC_TYPE_SIZE_T

# Checks for library functions.
#AC_FUNC_ERROR_AT_LINE
#AC_FUNC_MALLOC
#AC_FUNC_REALLOC
#AC_FUNC_VPRINTF
#AC_CHECK_FUNCS([floor memset sqrt strchr strdup strtol])

AC_DEFUN([ACX_PTHREAD], [
AC_REQUIRE([AC_CANONICAL_HOST])
AC_LANG_SAVE
AC_LANG_C
acx_pthread_ok=no

# We used to check for pthread.h first, but this fails if pthread.h
# requires special compiler flags (e.g. on True64 or Sequent).
# It gets checked for in the link test anyway.

# First of all, check if the user has set any of the PTHREAD_LIBS,
# etcetera environment variables, and if threads linking works using
# them:
if test x"$PTHREAD_LIBS$PTHREAD_CFLAGS" != x; then
        save_CFLAGS="$CFLAGS"
        CFLAGS="$CFLAGS $PTHREAD_CFLAGS"
        save_LIBS="$LIBS"
        LIBS="$PTHREAD_LIBS $LIBS"
        AC_MSG_CHECKING([for pthread_join in LIBS=$PTHREAD_LIBS with CFLAGS=$PTHREAD_CFLAGS])
        AC_TRY_LINK_FUNC(pthread_join, acx_pthread_ok=yes)
        AC_MSG_RESULT($acx_pthread_ok)
        if test x"$acx_pthread_ok" = xno; then
                PTHREAD_LIBS=""
                PTHREAD_CFLAGS=""
        fi
        LIBS="$save_LIBS"
        CFLAGS="$save_CFLAGS"
fi

# We must check for the threads library under a number of different
# names; the ordering is very important because some systems
# (e.g. DEC) have both -lpthread and -lpthreads, where one of the
# libraries is broken (non-POSIX).

# Create a list of thread flags to try.  Items starting with a "-" are
# C compiler flags, and other items are library names, except for "none"
# which indicates that we try without any flags at all.

acx_pthread_flags="pthreads none -Kthread -kthread lthread -pthread -pthreads -mthreads pthread --thread-safe -mt"

# The ordering *is* (sometimes) important.  Some notes on the
# individual items follow:

# pthreads: AIX (must check this before -lpthread)
# none: in case threads are in libc; should be tried before -Kthread and
#       other compiler flags to prevent continual compiler warnings
# -Kthread: Sequent (threads in libc, but -Kthread needed for pthread.h)
# -kthread: FreeBSD kernel threads (preferred to -pthread since SMP-able)
# lthread: LinuxThreads port on FreeBSD (also preferred to -pthread)
# -pthread: Linux/gcc (kernel threads), BSD/gcc (userland threads)
# -pthreads: Solaris/gcc
# -mthreads: Mingw32/gcc, Lynx/gcc
# -mt: Sun Workshop C (may only link SunOS threads [-lthread], but it
#      doesn't hurt to check since this sometimes defines pthreads too;
#      also defines -D_REENTRANT)
# pthread: Linux, etcetera
# --thread-safe: KAI C++

case "${host_cpu}-${host_os}" in
        *solaris*)

        # On Solaris (at least, for some versions), libc contains stubbed
        # (non-functional) versions of the pthreads routines, so link-based
        # tests will erroneously succeed.  (We need to link with -pthread or
        # -lpthread.)  (The stubs are missing pthread_cleanup_push, or rather
        # a function called by this macro, so we could check for that, but
        # who knows whether they'll stub that too in a future libc.)  So,
        # we'll just look for -pthreads and -lpthread first:

        acx_pthread_flags="-pthread -pthreads pthread -mt $acx_pthread_flags"
        ;;
esac

if test x"$acx_pthread_ok" = xno; then
for flag in $acx_pthread_flags; do

        case $flag in
                none)
                AC_MSG_CHECKING([whether pthreads work without any flags])
                ;;

                -*)
                AC_MSG_CHECKING([whether pthreads work with $flag])
                PTHREAD_CFLAGS="$flag"
                ;;

                *)
                AC_MSG_CHECKING([for the pthreads library -l$flag])
                PTHREAD_LIBS="-l$flag"
                ;;
        esac

        save_LIBS="$LIBS"
        save_CFLAGS="$CFLAGS"
        LIBS="$PTHREAD_LIBS $LIBS"
        CFLAGS="$CFLAGS $PTHREAD_CFLAGS"

        # Check for various functions.  We must include pthread.h,
        # since some functions may be macros.  (On the Sequent, we
        # need a special flag -Kthread to make this header compile.)
        # We check for pthread_join because it is in -lpthread on IRIX
        # while pthread_create is in libc.  We check for pthread_attr_init
        # due to DEC craziness with -lpthreads.  We check for
        # pthread_cleanup_push because it is one of the few pthread
        # functions on Solaris that doesn't have a non-functional libc stub.
        # We try pthread_create on general principles.
        AC_TRY_LINK([#include <pthread.h>],
                    [pthread_t th; pthread_join(th, 0);
                     pthread_attr_init(0); pthread_cleanup_push(0, 0);
                     pthread_create(0,0,0,0); pthread_cleanup_pop(0); ],
                    [acx_pthread_ok=yes])

        LIBS="$save_LIBS"
        CFLAGS="$save_CFLAGS"

        AC_MSG_RESULT($acx_pthread_ok)
        if test "x$acx_pthread_ok" = xyes; then
                break;
        fi

        PTHREAD_LIBS=""
        PTHREAD_CFLAGS=""
done
fi

# Various other checks:
if test "x$acx_pthread_ok" = xyes; then
        save_LIBS="$LIBS"
        LIBS="$PTHREAD_LIBS $LIBS"
        save_CFLAGS="$CFLAGS"
        CFLAGS="$CFLAGS $PTHREAD_CFLAGS"

        # Detect AIX lossage: threads are created detached by default
        # and the JOINABLE attribute has a nonstandard name (UNDETACHED).
        AC_MSG_CHECKING([for joinable pthread attribute])
        AC_TRY_LINK([#include <pthread.h>],
                    [int attr=PTHREAD_CREATE_JOINABLE;],
                    ok=PTHREAD_CREATE_JOINABLE, ok=unknown)
        if test x"$ok" = xunknown; then
                AC_TRY_LINK([#include <pthread.h>],
                            [int attr=PTHREAD_CREATE_UNDETACHED;],
                            ok=PTHREAD_CREATE_UNDETACHED, ok=unknown)
        fi
        if test x"$ok" != xPTHREAD_CREATE_JOINABLE; then
                AC_DEFINE(PTHREAD_CREATE_JOINABLE, $ok,
                          [Define to the necessary symbol if this constant
                           uses a non-standard name on your system.])
        fi
        AC_MSG_RESULT(${ok})
        if test x"$ok" = xunknown; then
                AC_MSG_WARN([we do not know how to create joinable pthreads])
        fi

        AC_MSG_CHECKING([if more special flags are required for pthreads])
        flag=no
        case "${host_cpu}-${host_os}" in
                *-aix* | *-freebsd*)     flag="-D_THREAD_SAFE";;
                *solaris* | *-osf* | *-hpux*) flag="-D_REENTRANT";;
        esac
        AC_MSG_RESULT(${flag})
        if test "x$flag" != xno; then
                PTHREAD_CFLAGS="$flag $PTHREAD_CFLAGS"
        fi

        LIBS="$save_LIBS"
        CFLAGS="$save_CFLAGS"

        # More AIX lossage: must compile with cc_r
        AC_CHECK_PROG(PTHREAD_CC, cc_r, cc_r, ${CC})
else
        PTHREAD_CC="$CC"
fi

AC_SUBST(PTHREAD_LIBS)
AC_SUBST(PTHREAD_CFLAGS)
AC_SUBST(PTHREAD_CC)

# Finally, execute ACTION-IF-FOUND/ACTION-IF-NOT-FOUND:
if test x"$acx_pthread_ok" = xyes; then
        ifelse([$1],,AC_DEFINE(HAVE_PTHREAD,1,[Define if you have POSIX threads libraries and header files.]),[$1])
        :
else
        acx_pthread_ok=no
        $2
fi
AC_LANG_RESTORE
])dnl ACX_PTHREAD

dnl do we need to specify -lm explicitly?
AC_CHECK_FUNC(sin,,[AC_CHECK_LIB(m,sin)])

dnl zlib is required
AC_CHECK_LIB(z,deflate,
  [LIBS="-lz $LIBS"
   AC_DEFINE(HAVE_LIBZ, 1, [ Define if you have zlib. ])],
  [AC_MSG_WARN([zlib is required - see http://www.gzip.org/zlib/])])

dnl libpng is required
AC_ARG_WITH(png,dnl
[  --with-png=DIR          where to find the png library],,
  withval=yes)

# libpng12.so is a nightmare. We have to test separately for
# libpng12-config and libpng-config. If we find libpng12-config,
# we don't look for the other, which will just be a symlink to it,
# confusing autoconf into thinking the library itself will be named -lpng,
# which it won't be. Awful. This code will suffice unless the libpng
# authors decide to do this AGAIN. Which I really hope they won't. TBB

if test "$withval" != no; then
  AC_PATH_PROG([LIBPNG12_CONFIG], [libpng12-config])
  AC_PATH_PROG([LIBPNG_CONFIG], [libpng-config])
  if test -n "$LIBPNG12_CONFIG"; then
    libpng_CPPFLAGS=`libpng12-config --cflags`
    # should be --ldopts, but it's currently broken
    libpng_LDFLAGS=`libpng12-config --ldflags`
    libpng_LDFLAGS=`echo " $libpng_LDFLAGS" | sed 's/ -l[[^ ]][[^ ]]*//g'`
  elif test -n "$LIBPNG_CONFIG"; then
    libpng_CPPFLAGS=`libpng-config --cflags`
    # should be --ldopts, but it's currently broken
    libpng_LDFLAGS=`libpng-config --ldflags`
    libpng_LDFLAGS=`echo " $libpng_LDFLAGS" | sed 's/ -l[[^ ]][[^ ]]*//g'`
  elif test -d "$withval"; then
    libpng_CPPFLAGS="-I$withval/include"
    libpng_LDFLAGS="-L$withval/lib"
  fi

  AC_CHECK_HEADERS(png.h,,
    [AC_CHECK_HEADERS(libpng/png.h,
       [CPPFLAGS="$CPPFLAGS $libpng_CPPFLAGS/libpng"],
       [CPPFLAGS="$_cppflags"])])

  _ldflags="$LDFLAGS"
  LDFLAGS="$libpng_LDFLAGS $LDFLAGS"

  if test -n "$LIBPNG12_CONFIG"; then
    AC_CHECK_LIB(png12,png_create_read_struct,
      [LIBS="-lpng12 $LIBS"
       FEATURES="GD_PNG $FEATURES"
       AC_DEFINE(HAVE_LIBPNG, 1, [ Define if you have the png library. ])],
      [LDFLAGS="$_ldflags"  
       AC_MSG_WARN([libpng is required - see http://www.libpng.org/pub/png/])])
  elif test -n "$LIBPNG_CONFIG"; then
    AC_CHECK_LIB(png,png_create_read_struct,
      [LIBS="-lpng $LIBS" 
       FEATURES="GD_PNG $FEATURES"
       AC_DEFINE(HAVE_LIBPNG, 1, [ Define if you have the png library. ])],
      [LDFLAGS="$_ldflags"  
       AC_MSG_WARN([libpng is required - see http://www.libpng.org/pub/png/])])
  else
    AC_CHECK_LIB(png,png_create_read_struct,
      [LIBS="-lpng $LIBS" 
       FEATURES="GD_PNG $FEATURES"
       AC_DEFINE(HAVE_LIBPNG, 1, [ Define if you have the png library. ])],
      [LDFLAGS="$_ldflags"  
       AC_MSG_WARN([libpng is required - see http://www.libpng.org/pub/png/])])
  fi
else
  ac_cv_lib_png_png_create_read_struct=no
  AC_MSG_WARN([libpng is required - see http://www.libpng.org/pub/png/])
fi

#TBB: png12's png_create_read_struct is just as good 
#2.0.23: should be = not ==  
if test "$ac_cv_lib_png12_png_create_read_struct" = yes; then
  ac_cv_lib_png_png_create_read_struct=yes
fi 

dnl FreeType configure tests snarfed from libwmf ..
AC_ARG_WITH(freetype,dnl
[  --with-freetype=DIR     where to find the freetype 2.x library],
  [if test "$withval" != no; then
     if test "$withval" != yes; then
       FREETYPE_DIR=$withval
     fi
   fi])

if test "$withval" != no; then

  if test -n "$FREETYPE_DIR"; then
    AC_PATH_PROG(FREETYPE_CONFIG,freetype-config,,[$FREETYPE_DIR/bin:$PATH])
  else
    AC_PATH_PROG(FREETYPE_CONFIG,freetype-config)
  fi

  if test -n "$FREETYPE_CONFIG"; then
    if test -n "$FREETYPE_DIR"; then
      libft_INCLUDES="`$FREETYPE_CONFIG --cflags` -I$FREETYPE_DIR/include"
    else
      libft_INCLUDES=`$FREETYPE_CONFIG --cflags`
    fi
    libft_LDFLAGS=`$FREETYPE_CONFIG --libs`
    libft_LDFLAGS=`echo " $libft_LDFLAGS" |sed 's/ -l[[^ ]][[^ ]]*//g'`
  else
    if test -n "$FREETYPE_DIR"; then
      libft_INCLUDES="-I$FREETYPE_DIR/include/freetype2 -I$FREETYPE_DIR/include"
      libft_LDFLAGS="-L$FREETYPE_DIR/lib"
    else
      libft_INCLUDES=""
      libft_LDFLAGS=""
    fi
  fi

  _cppflags="$CPPFLAGS"
  _ldflags="$LDFLAGS"
  CPPFLAGS="$libft_INCLUDES $CPPFLAGS"
  LDFLAGS="$libft_LDFLAGS $LDFLAGS"
  SAVE_FEATURES=$FEATURES
  AC_CHECK_LIB(freetype,FT_Init_FreeType,
    [LIBS="-lfreetype $LIBS" 
     FEATURES="GD_FREETYPE $FEATURES"
     AC_DEFINE(HAVE_LIBFREETYPE, 1, [ Define if you have the freetype library. ])],
    [LDFLAGS="$_ldflags"
     AC_MSG_WARN([not found - disabling freetype support])
     FEATURES=$SAVE_FEATURES])

  #TBB: 2.0.26: newer gd versions require ft2build.h
  AC_CHECK_HEADERS(ft2build.h,,
      [AC_MSG_RESULT([no])
      FEATURES=$SAVE_FEATURES
      ac_cv_lib_freetype_FT_Init_FreeType=no
      CPPFLAGS="$_cppflags"])

else
  ac_cv_lib_freetype_FT_Init_FreeType=no
fi

dnl check for libfontconfig by default
AC_ARG_WITH(fontconfig,dnl
[  --with-fontconfig=DIR         where to find the fontconfig library],
  [if test -d "$withval"; then
     LDFLAGS="$LDFLAGS -L$withval/lib"
     CPPFLAGS="$CPPFLAGS -I$withval/include"
   fi],
  withval=yes)

if test "$withval" != no; then
  AC_CHECK_LIB(fontconfig,FcInit,
    [LIBS="-lfontconfig $LIBS" 
     FEATURES="GD_FONTCONFIG $FEATURES"
     AC_DEFINE(HAVE_LIBFONTCONFIG, 1, [ Define if you have the fontconfig library. ])])
else
  ac_cv_lib_fontconfig_FcInit=no
fi

dnl check for libjpeg by default
AC_ARG_WITH(jpeg,dnl
[  --with-jpeg=DIR         where to find the jpeg library],
  [if test -d "$withval"; then
     LDFLAGS="$LDFLAGS -L$withval/lib"
     CPPFLAGS="$CPPFLAGS -I$withval/include"
   fi],
  withval=yes)

if test "$withval" != no; then
  AC_CHECK_LIB(jpeg,jpeg_set_defaults,
    [LIBS="-ljpeg $LIBS" 
     FEATURES="GD_JPEG $FEATURES"
     AC_DEFINE(HAVE_LIBJPEG, 1, [ Define if you have the jpeg library. ])])
else
  ac_cv_lib_jpeg_jpeg_set_defaults=no
fi

dnl check for libXpm by default
AC_ARG_WITH(xpm,dnl
[  --with-xpm=DIR          where to find the xpm library],
  [if test -d "$withval"; then
     LDFLAGS="$LDFLAGS -L$withval/lib"
     CPPFLAGS="$CPPFLAGS -I$withval/include"
   fi],
  withval=yes)

if test "$withval" != no; then
  AC_CHECK_LIB(Xpm,XpmReadFileToXpmImage,
    [LIBS="-lXpm -lX11 $LIBS"
     FEATURES="GD_XPM $FEATURES"
     AC_DEFINE(HAVE_LIBXPM, 1, [ Define if you have the Xpm library. ])],,
    [-lX11])
else
  ac_cv_lib_Xpm_XpmReadFileToXpmImage=no
fi

ACX_PTHREAD()

dnl report configuration
AC_MSG_RESULT([
** Configuration summary for $PACKAGE $VERSION:

   Support for PNG library:          $ac_cv_lib_png_png_create_read_struct
   Support for JPEG library:         $ac_cv_lib_jpeg_jpeg_set_defaults
   Support for Freetype 2.x library: $ac_cv_lib_freetype_FT_Init_FreeType
   Support for Fontconfig library:   $ac_cv_lib_fontconfig_FcInit
   Support for Xpm library:          $ac_cv_lib_Xpm_XpmReadFileToXpmImage
   Support for pthreads:             $acx_pthread_ok
])

AC_CONFIG_FILES([Makefile config/Makefile config/gdlib-config test/Makefile])

AC_OUTPUT
