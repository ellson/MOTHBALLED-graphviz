# $Id$ $Revision$

# Process this file with autoconf to produce a configure script
AC_PREREQ([2.61])

# Package name and version number, (as distinct from shared library version)
AC_INIT(graphviz, 2.17)

dnl ===========================================================================

# libtool shared library version for main libs (cdt, graph, gvc)

# Increment if the interface has additions, changes, removals.
#LT_CURRENT=4

# Increment any time the source changes; set to
# 0 if you increment CURRENT
#LT_REVISION=0

# Increment if any interfaces have been added; set to 0
# if any interfaces have been removed. removal has
# precedence over adding, so set to 0 if both happened.
#LT_AGE=0

#VERSION_INFO="$LT_CURRENT:$LT_REVISION:$LT_AGE"
VERSION_INFO="4:0:0"
AC_SUBST(VERSION_INFO)
dnl ===========================================================================

# libtool shared library version for plugins

# Increment if the interface has additions, changes, removals.
GVPLUGIN_CURRENT=5

# Increment any time the source changes; set to
# 0 if you increment CURRENT
GVPLUGIN_REVISION=0

# Increment if any interfaces have been added; set to 0
# if any interfaces have been removed. removal has
# precedence over adding, so set to 0 if both happened.
GVPLUGIN_AGE=0

GVPLUGIN_VERSION_INFO="$GVPLUGIN_CURRENT:$GVPLUGIN_REVISION:$GVPLUGIN_AGE"
AC_SUBST(GVPLUGIN_VERSION_INFO)

dnl ===========================================================================


AC_CONFIG_SRCDIR(configure.ac)
AC_CONFIG_AUX_DIR(config)

dnl -----------------------------------
dnl Checks for system type

AC_CANONICAL_HOST

# This nonsense drives me crazy!!
case "${host_os}" in
  *irix* | *nonstopux* )
    GVPLUGIN_VERSION=`expr $GVPLUGIN_CURRENT + 1`
    ;;
  * )
    GVPLUGIN_VERSION=$GVPLUGIN_CURRENT
    ;;
esac
AC_DEFINE_UNQUOTED(GVPLUGIN_VERSION,$GVPLUGIN_VERSION,[Compatibility version number for plugins.])
AC_SUBST(GVPLUGIN_VERSION)

LIBPOSTFIX=
case "${host_os}" in
  *linux* )
    case "${host_cpu}" in
      powerpc64 | s390x | x86_64 )
        LIBPOSTFIX="64"
        ;;
    esac
    ;;
esac

UWIN=no
CYGWIN=no
MINGW32=no
DARWIN=no
case "${host_os}" in
  *uwin* )
	UWIN=yes
	DEFAULT_FONTPATH="C:/WINDOWS/FONTS;C:/WINNT/Fonts;C:/winnt/fonts"
	CFLAGS="${CFLAGS} -DWIN32 -DMSWIN32"
	CXXFLAGS="${CXXFLAGS} -DWIN32 -DMSWIN32"
	PATHSEPARATOR=";"
        NO_POSTSCRIPT_ALIAS=1
	;;
  *cygwin* )
	CYGWIN=yes
	DEFAULT_FONTPATH="C:/WINDOWS/FONTS;C:/WINNT/Fonts;C:/winnt/fonts"
	PATHSEPARATOR=";"
        NO_POSTSCRIPT_ALIAS=1
	AC_DEFINE(NO_POSTSCRIPT_ALIAS,1,[Alias PostScript font names to URW type 1 fonts])
	;;
  *mingw32* )
	MINGW32=yes
	DEFAULT_FONTPATH="C:/WINDOWS/FONTS;C:/WINNT/Fonts;C:/winnt/fonts"
	PATHSEPARATOR=";"
        NO_POSTSCRIPT_ALIAS=1
	;;
  *darwin* )
	DEFAULT_FONTPATH="~/Library/Fonts:/Library/Fonts:/Network/Library/Fonts:/System/Library/Fonts"
	PATHSEPARATOR=":"
        NO_POSTSCRIPT_ALIAS=1
	AC_DEFINE_UNQUOTED(DARWIN_DYLIB,"$DARWIN_DYLIB",[Define for Darwin-style shared library names.])
	;;
  * ) # This sucks!  Whatever happened to standards for file system layout?
	DEFAULT_FONTPATH="/usr/share/fonts/default/Type1:/usr/X11R6/lib/X11/fonts/TrueType:/usr/X11R6/lib/X11/fonts/truetype:/usr/X11R6/lib/X11/fonts/TTF:/usr/share/fonts/TrueType:/usr/share/fonts/truetype:/usr/openwin/lib/X11/fonts/TrueType:/usr/X11R6/lib/X11/fonts/Type1:/usr/common/share/fonts/ttf";
	PATHSEPARATOR=":"
        NO_POSTSCRIPT_ALIAS=0
	;;
esac
AC_SUBST(CYGWIN)
AC_SUBST(MINGW32)
AC_SUBST(DARWIN)
AC_DEFINE_UNQUOTED(DEFAULT_FONTPATH,"$DEFAULT_FONTPATH",[Path to TrueType fonts.])
AC_DEFINE_UNQUOTED(PATHSEPARATOR,"$PATHSEPARATOR",[Path separator character.])
AC_DEFINE_UNQUOTED(NO_POSTSCRIPT_ALIAS,$NO_POSTSCRIPT_ALIAS,[Postscript fontnames.])
AM_CONDITIONAL(WITH_WIN32, [test "x$UWIN" = "xyes"])

DEFAULT_DPI=96
AC_DEFINE_UNQUOTED(DEFAULT_DPI,$DEFAULT_DPI,Default DPI.)

AM_INIT_AUTOMAKE(AC_PACKAGE_NAME, AC_PACKAGE_VERSION)
AM_CONFIG_HEADER(config.h)

# put BUILDDATE in its own file so that only targets that use it get rebuilt
echo "#define BUILDDATE \"`date -u`\"" > builddate.h

# only update ast_common.h (for GNU builds) from master if changed
if ! `cmp -s $srcdir/ast_common.h.in ast_common.h`; then
	cp $srcdir/ast_common.h.in ast_common.h;
fi

AC_PREFIX_DEFAULT([/usr/local])
if test "x${prefix}" = "xNONE"; then
	prefix=${ac_default_prefix}
	AC_SUBST(prefix)
fi

# automatically check for other dependencies in $prefix first
if test "x${prefix}" != "x/usr"; then
	CPPFLAGS="$CPPFLAGS -I${prefix}/include"
	LDFLAGS="$LDFLAGS -L${prefix}/lib"
	PKG_CONFIG_PATH="$prefix/lib/pkgconfig$PATHSEPARATOR$PKG_CONFIG_PATH"
	export PKG_CONFIG_PATH
fi

# Set of all available languages
# ALL_LINGUAS=""

dnl -----------------------------------
# Static/Shared binaries 

AC_ARG_ENABLE(static,
        [AC_HELP_STRING([--enable-static], [build static executable])])
if test "x$enable_static" = "xyes"; then
	AC_ENABLE_STATIC
	AC_DEFINE(ENABLE_STATIC,1,[Define if you want statically linked executables])
else
	AC_DISABLE_STATIC
fi
AM_CONDITIONAL(ENABLE_STATIC, [test "x$enable_static" = "xyes"])

AC_ARG_ENABLE(shared,
        [AC_HELP_STRING([--enable-shared], [build shared executable])])
if test "x$enable_shared" != "xno"; then
	AC_ENABLE_SHARED
	AC_DEFINE(ENABLE_SHARED,1,[Define if you want shared lib executables])
else
	AC_DISABLE_SHARED
fi
AM_CONDITIONAL(ENABLE_SHARED, [test "x$enable_shared" = "xyes"])

dnl -----------------------------------
dnl checks for various programs

AC_ISC_POSIX
AM_PROG_LEX
AC_PROG_YACC
AC_PROG_AWK
AC_PROG_INSTALL
AC_PROG_LN_S
AC_CHECK_PROG(SED,sed,sed,false)
AC_CHECK_PROG(GREP,grep,grep,false)
AC_CHECK_PROG(SORT,sort,sort,false)
PKG_PROG_PKG_CONFIG

# are we building for windows?
# (these tests must follow AC_ISC_POSIX)
#AC_CYGWIN
#AC_MINGW32

dnl -----------------------------------
dnl checks for compilers

AC_PROG_CC
AC_PROG_CXX

# Workaround for native compilers
#  HP  : http://bugs.gnome.org/db/31/3163.html
#  DEC : Enable NaN/Inf

if test "${GCC}" != "yes" ; then
    case "${build}" in
        *-*-hpux* )
	    CFLAGS="${CFLAGS} -Wp,-H30000"
            ;;
        *-dec-osf* )
            CFLAGS="${CFLAGS} -ieee"
            ;;
	*-apple-darwin* )
	    CFLAGS="${CFLAGS} -fno-common"
	    ;;
	*-sgi-irix* )
	    ;;
    esac
else
    case "${CC}" in
	*icc* )
	    # -Wall causes relocation errors with icc
	    CFLAGS="${CFLAGS} -wd269"
	    # icc doesn't like -pipe
	    CFLAGS=`echo ${CFLAGS} | sed 's/ -pipe//'`
	    # icc doesn't like -ffast-math
	    CFLAGS=`echo ${CFLAGS} | sed 's/ -ffast-math//'`
	    # icc doesn't like -O2
	    CFLAGS=`echo ${CFLAGS} | sed 's/ -O2/ -O0/'`
	    ;;
        * )
            case "${build}" in
	        *-dec-osf* )
	            CFLAGS="${CFLAGS} -mieee -Wall"
	            ;;
                *alpha*-*-linux* )
                    CFLAGS="${CFLAGS} -mieee -Wall"
                    ;;
	        *-apple-darwin* )
	            CFLAGS="${CFLAGS} -fno-common -Wall"
	            ;;
	        *-freebsd* )
	            CFLAGS="${CFLAGS} -Wstrict-prototypes -Wpointer-arith -Wall -ffast-math"
		    ;;
	        * )
		    # -Wno-unused-parameter only needed for bug in gcc3
		    # this test from: http://cvs.auriga.wearlab.de/cgi-bin/cvsweb.cgi/dillo/configure.in?rev=1.90;content-type=text%2Fplain;cvsroot=dillo
		    if test "`$CC -v 2>&1 | $GREP 'version 3'`" != ""; then
		        CFLAGS="${CFLAGS} -Wno-unused-parameter"
		    fi
	            CFLAGS="${CFLAGS} -Wno-unknown-pragmas -Wstrict-prototypes -Wpointer-arith -Wall -ffast-math"
	            ;;
            esac
	    ;;
    esac
fi
# -ffast-math is incompatible with FP error trapping
#     ref: https://bugzilla.redhat.com/bugzilla/show_bug.cgi?id=175793
if `echo ${CFLAGS} | $GREP ffast-math >/dev/null`; then
    AC_DEFINE_UNQUOTED(NO_FPERR,1,[Define if no fpu error exception handling is required.])
fi

dnl -----------------------------------
dnl Check for various typedefs and provide substitutes if they don't exist.
                                                                                
AC_C_CONST
AC_TYPE_MODE_T
AC_TYPE_PID_T
AC_TYPE_SIZE_T
AC_TYPE_SSIZE_T
AC_TYPE_UID_T

dnl -----------------------------------
dnl Checks for header files

# AC_STDC_HEADERS
AC_CHECK_HEADERS(stdarg.h stddef.h stddef.h stdlib.h stdint.h malloc.h \
	search.h getopt.h pthread.h values.h float.h limits.h termios.h \
	errno.h time.h unistd.h fenv.h string.h strings.h inttypes.h setjmp.h \
	sys/time.h sys/times.h sys/types.h sys/select.h fpu_control.h \
	sys/fpu.h sys/socket.h sys/stat.h sys/mman.h \
	sys/ioctl.h sys/inotify.h langinfo.h libintl.h)
AC_HEADER_TIME
AC_HEADER_DIRENT
AC_HEADER_STDBOOL

# Check for st_blksize in struct stat
# AC_ST_BLKSIZE

# Internationalization macros
# AM_GNU_GETTEXT

# Check for socklen_t
AC_MSG_CHECKING([for socklen_t])
AC_CACHE_VAL(ac_cv_type_socklen_t,[AC_EGREP_CPP(changequote(<<,>>)dnl
<<(^|[^a-zA-Z_0-9])socklen_t[^a-zA-Z_0-9]>>dnl
changequote([,]),[
    #include <sys/types.h>
    #include <sys/socket.h>
    #if STDC_HEADERS
    #include <stdlib.h>
    #include <stddef.h>
    #endif
    ], ac_cv_type_socklen_t=yes, ac_cv_type_socklen_t=no)])
AC_MSG_RESULT($ac_cv_type_socklen_t)
if test $ac_cv_type_socklen_t = no; then
    AC_DEFINE(socklen_t, unsigned, [Define to unsigned if socklet_t is missing])
fi

# -----------------------------------
# Test if compiler supports bool
AC_MSG_CHECKING([for bool])
AC_TRY_COMPILE([
    #ifdef HAVE_STDBOOL_H>
    #include <stdbool.h>
    #endif
    ],[
    bool foo = true, bar = false;
    ],[
AC_MSG_RESULT(yes)
AC_DEFINE(HAVE_BOOL, 1,[Define to 1 if compiler supports bool])
    ],[
AC_MSG_RESULT(no)])

# -----------------------------------
# Test for direct I/O
AC_MSG_CHECKING([for struct dioattr])
AC_TRY_COMPILE([#include <sys/types.h>
#include <fcntl.h>
#include <stdio.h>],[
struct dioattr dio;],[
AC_MSG_RESULT(yes)
AC_DEFINE(HAVE_STRUCT_DIOATTR, 1,
[Define to 1 if you have struct dioattr])],[
AC_MSG_RESULT(no)])

dnl -----------------------------------
dnl Checks for -lm library

save_LIBS=$LIBS
AC_CHECK_LIB(m, main, [MATH_LIBS="-lm"])
AC_SUBST(MATH_LIBS)

# Check for functions that might need -lm
LIBS="$LIBS $MATH_LIBS"
AC_CHECK_FUNCS([feenableexcept fesetenv])

LIBS=$save_LIBS

dnl -----------------------------------
dnl INCLUDES and LIBS for libgen (provides basename function)

save_CPPFLAGS=$CPPFLAGS
save_LDFLAGS=$LDFLAGS
AC_CHECK_HEADER(libgen.h,
        AC_CHECK_LIB(gen,basename,
                [LIBGEN_LIBS="-lgen"
                AC_DEFINE_UNQUOTED(HAVE_LIBGEN,1,[Define if the LIBGEN library has the basename feature])]),
        AC_MSG_WARN(libgen.h header not available))
LDFLAGS=$save_LDFLAGS
CPPFLAGS=$save_CPPFLAGS
AC_SUBST(LIBGEN_LIBS)

# -----------------------------------

# Checks for library functions
AC_FUNC_ALLOCA

AC_CHECK_FUNCS([lrand48 drand48 srand48 setmode setenv getenv \
	__freadable _sysconf getrusage strerror cbrt lsqrt vsnprintf \
	strtoul strtoll strtoull uname memset nl_langinfo pow sqrt \
	strchr strdup strerror strstr])

AC_REPLACE_FUNCS([strcasecmp strncasecmp])

dnl -----------------------------------
dnl libtool ltdl on-demand plugin loading

AC_ARG_ENABLE(ltdl,
  [AC_HELP_STRING([--enable-ltdl], [support on-demand plugin loading])])
if test "x$enable_ltdl" != "xno"; then
  AC_DEFINE(ENABLE_LTDL,1,[Define if you want on-demand plugin loading])
  AC_CONFIG_SUBDIRS(libltdl)
  AC_LIBTOOL_DLOPEN
  AC_LIBLTDL_INSTALLABLE
fi
AM_CONDITIONAL(ENABLE_LTDL, [test "x$enable_ltdl" != "xno"])
AC_SUBST(INCLTDL)
AC_SUBST(LIBLTDL)
AM_PROG_LIBTOOL

# Checks for libraries.
#AC_CHECK_LIB([ICE], [main])
#AC_CHECK_LIB([SM], [main])
#AC_CHECK_LIB([Xaw], [main])
#AC_CHECK_LIB([Xpm], [main])
#AC_CHECK_LIB([art_lgpl_2], [main])
#AC_CHECK_LIB([cairo], [main])
#AC_CHECK_LIB([dl], [main])
#AC_CHECK_LIB([fl], [main])
#AC_CHECK_LIB([fontconfig], [main])
#AC_CHECK_LIB([freetype], [main])
#AC_CHECK_LIB([iconv], [main])
#AC_CHECK_LIB([intl], [main])
#AC_CHECK_LIB([jpeg], [main])
#AC_CHECK_LIB([m], [main])
#AC_CHECK_LIB([perl], [main])
#AC_CHECK_LIB([png], [main])
#AC_CHECK_LIB([popt], [main])
#AC_CHECK_LIB([xml2], [main])
#AC_CHECK_LIB([z], [main])

dnl -----------------------------------
dnl old codegens

AC_ARG_WITH(codegens,
  [AC_HELP_STRING([--with-codegens=yes], [use old codegens])],
  [],[with_codegens=yes])
if test "x$with_codegens" = "xyes"; then
  AC_DEFINE(WITH_CODEGENS,1,[Define if you want the old codegens])
fi
AM_CONDITIONAL(WITH_CODEGENS, [test "x$with_codegens" = "xyes"])

dnl -----------------------------------
dnl ElectricFence support for debugging

AC_ARG_WITH(efence,
   [AC_HELP_STRING([--with-efence=no], [use efence for debugging memory use])],
   [],[with_efence=no])
if test "x$with_efence" = "xyes"; then
	LIBS="$LIBS -lefence"
fi

dnl -----------------------------------
dnl Don't compile X programs (and emit a warning message)
dnl if X isn't available.
dnl Checks for X header files.

AC_PATH_XTRA
if test "x$no_x" = xyes; then
  AC_MSG_WARN([X11 not available.])
  X_SUBDIRS=""
fi
AM_CONDITIONAL(WITH_X, [test "x$no_x" != "xyes"])

dnl -----------------------------------
dnl check for SWIG - needed for script-language bindings

AC_ARG_ENABLE(swig,
  [AC_HELP_STRING([--enable-swig=yes], [swig-generated language bindings])],
  [],[enable_swig=yes])
if test "x$enable_swig" = "xyes"; then
AC_CHECK_PROG(SWIG,swig,swig)
if test "x$SWIG" != "x"; then
  SWIG_VERSION=`$SWIG -version 2>&1 | $GREP Version | cut -d ' ' -f 3`
  SWIG_VERSION_MAJOR=`echo $SWIG_VERSION | cut -d '.' -f 1`
  SWIG_VERSION_MINOR=`echo $SWIG_VERSION | cut -d '.' -f 2`
  if test $SWIG_VERSION_MAJOR -lt 2; then
    if test $SWIG_VERSION_MAJOR -lt 1; then
	SWIG=
    else
      if test $SWIG_VERSION_MINOR -lt 3; then
	SWIG=
      fi
    fi
  fi
  if test "x$SWIG" = "x"; then
    AC_MSG_WARN([The version of swig is too old. The language bindings will not be built])
  fi
fi
AC_SUBST(SWIG_VERSION)
fi
AM_CONDITIONAL(WITH_SWIG, [test "x$SWIG" != "x"])

dnl -----------------------------------
dnl INCLUDES and LIBS for C#

if test "x$SWIG" != "x"; then
AC_ARG_ENABLE(sharp,
  [AC_HELP_STRING([--enable-sharp=yes], [C# language bindings])],
  [],[enable_sharp=yes])
if test "x$enable_sharp" = "xyes"; then
if test `$SWIG -help 2>&1 | $GREP -c '\-csharp *- Generate'` = 0; then
	SHARP=
	AC_MSG_WARN([swig does not support -csharp. The SHARP packages will not be built])
else
AC_CHECK_PROG(SHARP,mcs,mcs)
SHARP_INCLUDES=
SHARP_LIBS=
AC_SUBST(SHARP_INCLUDES)
AC_SUBST(SHARP_LIBS)
fi
fi
fi
AM_CONDITIONAL(WITH_SHARP, [test "x$SHARP" != "x"])

dnl -----------------------------------
dnl INCLUDES and LIBS for GUILE

if test "x$SWIG" != "x"; then
AC_ARG_ENABLE(guile,
  [AC_HELP_STRING([--enable-guile=yes], [guile language bindings])],
  [],[enable_guile=yes])
if test "x$enable_guile" = "xyes"; then
if test `$SWIG -help 2>&1 | $GREP -c '\-guile *- Generate'` = 0; then
	GUILE=
	AC_MSG_WARN([swig does not support -guile. The GUILE packages will not be built])
else
AC_CHECK_PROG(GUILE,guile,guile)
if test "x$GUILE" != "x"; then
  GUILE_VERSION=`$GUILE --version | head -1 | cut -d ' ' -f 2`
  GUILE_VERSION_MAJOR=`echo $GUILE_VERSION | cut -d '.' -f 1`
  GUILE_VERSION_MINOR=`echo $GUILE_VERSION | cut -d '.' -f 2`
  if test $GUILE_VERSION_MAJOR -lt 2; then
    if test $GUILE_VERSION_MAJOR -lt 1; then
      GUILE=
    else
      if test $GUILE_VERSION_MINOR -lt 4; then
	GUILE=
      fi
    fi
  fi
  if test "x$GUILE" = "x"; then
    AC_MSG_WARN([The version of guile is too old. The guile language binding will not be built])
  else
    GUILE_INCLUDES=
    GUILE_LIBS=
    AC_CHECK_HEADER(libguile.h,,[
    AC_MSG_WARN([Unable to find header libguile.h. The Guile packages will not be built])
      GUILE=
    ])
    AC_SUBST(GUILE_INCLUDES)
    AC_SUBST(GUILE_LIBS)
  fi
fi
fi
fi
fi
AM_CONDITIONAL(WITH_GUILE, [test "x$GUILE" != "x"])

dnl -----------------------------------
dnl INCLUDES and LIBS for IO   (disbled until supported by swig)

if test "x$SWIG" != "x"; then
AC_ARG_ENABLE(io,
  [AC_HELP_STRING([--enable-io=no], [io language bindings])],
  [], [enable_io=no])
if test "x$enable_io" = "xyes"; then
if test `$SWIG -help 2>&1 | $GREP -c '\-io *- Generate'` = 0; then
	IO=
	AC_MSG_WARN([swig does not support -io. The IO packages will not be built])
else
AC_CHECK_PROG(IO,io,io)
IO_INCLUDES=
IO_LIBS=
AC_SUBST(IO_INCLUDES)
AC_SUBST(IO_LIBS)
fi
fi
fi
AM_CONDITIONAL(WITH_IO, [test "x$IO" != "x"])

dnl -----------------------------------
dnl INCLUDES and LIBS for JAVA

if test "x$SWIG" != "x"; then
AC_ARG_ENABLE(java,
  [AC_HELP_STRING([--enable-java=yes], [java language bindings])],
  [], [enable_java=yes])
if test "x$enable_java" = "xyes"; then
if test `$SWIG -help 2>&1 | $GREP -c '\-java *- Generate'` = 0; then
	JAVA=
	AC_MSG_WARN([swig does not support -java. The JAVA packages will not be built])
else
AC_CHECK_PROG(JAVA,java,java)
JAVA_INCLUDES=
JAVA_LIBS=
AC_CHECK_HEADER(jni.h,,[
  AC_MSG_WARN([Unable to find header jni.h. The JAVA packages will not be built])
  JAVA=
  ])
AC_SUBST(JAVA_INCLUDES)
AC_SUBST(JAVA_LIBS)
fi
fi
fi
AM_CONDITIONAL(WITH_JAVA, [test "x$JAVA" != "x"])

dnl -----------------------------------
dnl INCLUDES and LIBS for LUA

if test "x$SWIG" != "x"; then
AC_ARG_ENABLE(lua,
  [AC_HELP_STRING([--enable-lua=yes], [lua language bindings])],
  [], [enable_lua=yes])
if test "x$enable_lua" = "xyes"; then
if test `$SWIG -help 2>&1 | $GREP -c '\-lua *- Generate'` = 0; then
	LUA=
	AC_MSG_WARN([swig does not support -lua. The LUA packages will not be built])
	enable_lua=no
fi
fi
fi

if test "x$enable_lua" = "xyes" ; then
    ac_save_CPPFLAGS="$CPPFLAGS"
    ac_save_CFLAGS="$CFLAGS"
    ac_save_LDFLAGS="$LDFLAGS"

    AC_CHECK_PROG(LUA,lua,lua)

    LUA_INCLUDES=""
    LUA_LIBS=""

    if test -n "$lua_inc"; then
       CFLAGS="$CFLAGS -I$lua_inc"
       CPPFLAGS="$CPPFLAGS -I$lua_inc"
    fi
    if test -n "$lua_lib"; then
       LDFLAGS="$LDFLAGS -L$lua_lib"
    fi

    if test "x$LUA_INCLUDES" = "x" -o "x$LUA_LIBS" = "x" ; then
        PKGCONFIG=""
        AC_CHECK_PROGS(PKGCONFIG, pkg-config)
        if test "x$PKGCONFIG" != "x"; then
	    AC_MSG_CHECKING(for Lua headers and libraries with pkg-config)
	    echo
	    for l in "$lua_suffix" "" "51" "5.1" "50" "5.0" ; do
	        pkgconfig_lua_found=`$PKGCONFIG --exists lua$l 2>/dev/null`
	        if test "x$?" = "x0" ; then
		    LUA_VERSION=`$PKGCONFIG --modversion lua$l`
                    LUA_INCLUDES="$LUA_CFLAGS "`$PKGCONFIG --cflags lua$l`
                    LUA_LIBS="$LUA_LFLAGS "`$PKGCONFIG --libs lua$l`

		    pkgconfig_lualib_found=`$PKGCONFIG --exists lualib$l 2>/dev/null`
		    if test  "x$?" = "x0"; then
		       LUA_INCLUDES="$LUA_CFLAGS "`$PKGCONFIG --cflags lualib$l`
		       LUA_LIBS="$LUA_LFLAGS "`$PKGCONFIG --libs lualib$l`
		    fi
                    break
                fi
            done
        fi
    fi

    if test "x$LUA_INCLUDES" = "x" -o "x$LUA_LIBS" = "x" ; then
        LUACONFIG=""
        AC_CHECK_PROGS(LUACONFIG, lua-config lua-config51 lua-config5.1 lua-config50 lua-config5.0)
        if test "x$LUACONFIG" != "x" ; then
            AC_MSG_CHECKING(for Lua headers and libraries with lua-config)
	    echo
            LUA_INCLUDES=`$LUACONFIG --include`
            LUA_LIBS=`$LUACONFIG --libs`
	    LUA_VERSION="5.0.x"
        fi
    fi

    if test "x$LUA_INCLUDES" = "x" -o "x$LUA_LIBS" = "x" ; then
        AC_MSG_CHECKING(for Lua headers and libraries)
	echo
        AC_CHECK_HEADER(lua.h,ac_found_lua_header="yes",ac_found_lua_header="no")
        AC_CHECK_HEADER(lualib.h,ac_found_liblua_header="yes",ac_found_liblua_header="no")
        if test "x$ac_found_lua_header" = "xyes" -a "x$ac_found_liblua_header" = "xyes"; then
            LUA_INCLUDES="$CFLAGS"
        fi
        for l in "$lua_suffix" "" "51" "5.1" "50" "5.0" ; do
            AC_CHECK_LIB(lua$l,lua_call,ac_found_lua_lib="yes",ac_found_lua_lib="no")
            if test "x$ac_found_lua_lib" = "xyes" ; then
	        LUA_VERSION=">=5.1.0"

	        LUA_LIBS="$LDFLAGS -llua$l -lm"

		ac2_save_LDFLAGS="$LDFLAGS"
		LDFLAGS="$LDFLAGS -llua$l -lm"

		if echo "$host_os" | grep "^linux" 1>/dev/null 2>&1 ; then
		   LDFLAGS="$LDFLAGS -ldl"
		fi

		AC_CHECK_LIB(lualib$l,luaL_openlib,ac_found_liblua_lib="yes",ac_found_liblua_lib="no")
		if test "x$ac_found_liblua_lib" = "xyes" ; then
		   LUA_VERSION="5.0.x"

		   LUA_LIBS="$LUA_LFLAGS -llualib$l"

		   if echo "$host_os" | grep "^linux" 1>/dev/null 2>&1 ; then
		      LUA_LIBS="$LUA_LFLAGS -ldl"
		   fi
		fi

		LDFLAGS="$ac2_save_LDFLAGS"
		break
	    fi
        done
    fi

    AC_MSG_CHECKING(for Lua compiling and linking)
    LUA_TEST=`LT=luatest.c ; echo "#include <lua.h>" > $LT;  echo "#include <lualib.h>" >> $LT; echo "int main() { luaopen_base((lua_State *)lua_open()); return 0; }" >> $LT ; $CC -Wall $LT -o $LT.out $LUA_INCLUDES $LUA_LIBS $CFLAGS $LDFLAGS 1>/dev/null 2>&1 ; echo $?; rm -f $LT $LT.out 1>/dev/null 2>&1`
    if test "x$LUA_TEST" != "x0" ; then
        AC_MSG_RESULT(no)
        AC_MSG_WARN([
*** Lua (>=5.0) headers and/or libraries could not be found in your system.
*** Try to install liblua, liblualib and liblua-dev with your software package manager.
*** Graphviz will be built without Lua support.])
        enable_lua="no"
        not_found="$not_found lua"
    else
        AC_MSG_RESULT(yes)
    fi

    CFLAGS="$ac_save_CFLAGS"
    CPPFLAGS="$ac_save_CPPFLAGS"
    LDFLAGS="$ac_save_LDFLAGS"
fi

if test "x$enable_lua" = "xyes" ; then
    : # needed because if AC_SUBST is expanded to nothing, syntax error
    AC_SUBST(LUA_INCLUDES)
    AC_SUBST(LUA_LIBS)
    # Not needed for Graphviz: AC_DEFINE(PLUGIN_LUA)
fi

# --->8--- weechat --->8---

#echo "WE HAVE HERE: $enable_lua" > "/tmp/IAMAFOOL"
AM_CONDITIONAL(WITH_LUA, [test "x$enable_lua" = "xyes"])


dnl -----------------------------------
dnl INCLUDES and LIBS for OCAML

if test "x$SWIG" != "x"; then
AC_ARG_ENABLE(ocaml,
  [AC_HELP_STRING([--enable-ocaml=yes], [ocaml language bindings])],
  [], [enable_ocaml=yes])
if test "x$enable_ocaml" = "xyes"; then
if test `$SWIG -help 2>&1 | $GREP -c '\-ocaml *- Generate'` = 0; then
	OCAML=
	AC_MSG_WARN([swig does not support -ocaml. The OCAML packages will not be built])
else
AC_CHECK_PROG(OCAML,ocaml,ocaml)
AC_CHECK_PROG(OCAML_OPT,ocamlopt,ocamlopt)
OCAML_INCLUDES=-I/usr/lib$LIBPOSTFIX/ocaml
OCAML_LIBS=-L/usr/lib$LIBPOSTFIX/ocaml
save_CPPFLAGS=$CPPFLAGS
CPPFLAGS="$CPPFLAGS $OCAML_INCLUDES"
AC_CHECK_HEADER(caml/mlvalues.h,,[
  AC_MSG_WARN([Unable to find header caml/mlvalues.h. The OCAML packages will not be built])
  OCAML=
  ])
CPPFLAGS=$save_CPPFLAGS
AC_SUBST(OCAML_INCLUDES)
AC_SUBST(OCAML_LIBS)
fi
fi
fi
AM_CONDITIONAL(WITH_OCAML, [test "x$OCAML" != "x"])
AM_CONDITIONAL(WITH_OCAMLOPT, [test "x$OCAML_OPT" != "x"])

dnl -----------------------------------
dnl INCLUDES and LIBS for PERL

if test "x$SWIG" != "x"; then
AC_ARG_ENABLE(perl,
  [AC_HELP_STRING([--enable-perl=yes], [perl language bindings])],
  [], [enable_perl=yes])
if test "x$enable_perl" = "xyes"; then
AC_CHECK_PROG(PERL,perl,perl)
if test -n "$PERL" && $PERL -e 'exit !($] <= 5.004)' > /dev/null 2>&1 ; then
	PERL=
	AC_MSG_WARN([perl is too old. The PERL packages will not be built])
fi
if test `$SWIG -help 2>&1 | $GREP -c '\-perl *- Generate'` = 0; then
	PERL=
	AC_MSG_WARN([swig does not support -perl. The PERL packages will not be built])
fi
if test "x$PERL" != "x"; then
  PERL_ARCHLIB=`$PERL -e 'use Config; print $Config{archlib};'`
  PERL_INCLUDES=-I$PERL_ARCHLIB/CORE
  PERL_LIBS="-L$PERL_ARCHLIB/CORE -lperl"
  PERL_INSTALL_DIR="`$PERL -e 'use Config; print $Config{vendorarch};'`"
  save_CPPFLAGS=$CPPFLAGS
  CPPFLAGS="$CPPFLAGS $PERL_INCLUDES"
  AC_CHECK_HEADER(EXTERN.h,,[
    AC_MSG_WARN([Unable to find header EXTERN.h. The PERL packages will not be built])
    PERL=
    ])
  CPPFLAGS=$save_CPPFLAGS
fi
AC_SUBST(PERL_INCLUDES)
AC_SUBST(PERL_LIBS)
AC_SUBST(PERL_INSTALL_DIR)
fi
fi
AM_CONDITIONAL(WITH_PERL, [test "x$PERL" != "x"])

dnl -----------------------------------
dnl INCLUDES and LIBS for PHP

if test "x$SWIG" != "x"; then
AC_ARG_ENABLE(php,
  [AC_HELP_STRING([--enable-php=yes], [php language bindings])],
  [], [enable_php=yes])
if test "x$enable_php" = "xyes"; then
if test `$SWIG -help 2>&1 | $GREP -c '\-php5[[ ]]*- Generate'` = 0; then
	PHP=
	AC_MSG_WARN([swig does not support -php5. The PHP packages will not be built])
else
AC_CHECK_PROG(PHP,php,php)
if test -d /usr/include/php5; then
PHP_INCLUDES="-I/usr/include/php5 -I/usr/include/php5/main -I/usr/include/php5/Zend -I/usr/include/php5/TSRM"
else
PHP_INCLUDES="-I/usr/include/php -I/usr/include/php/main -I/usr/include/php/Zend -I/usr/include/php/TSRM"
fi
PHP_INSTALL_DIR="/usr/lib${LIBPOSTFIX}/php/modules"
PHP_INSTALL_DATADIR="/usr/share/php"
PHP_LIBS=
save_CPPFLAGS=$CPPFLAGS
CPPFLAGS="$CPPFLAGS $PHP_INCLUDES"
AC_CHECK_HEADER(zend.h,,[
  AC_MSG_WARN([Unable to find header zend.h. The PHP packages will not be built])
  PHP=
  ])
CPPFLAGS=$save_CPPFLAGS
AC_SUBST(PHP_INSTALL_DATADIR)
AC_SUBST(PHP_INSTALL_DIR)
AC_SUBST(PHP_INCLUDES)
AC_SUBST(PHP_LIBS)
fi
fi
fi
AM_CONDITIONAL(WITH_PHP, [test "x$PHP" != "x"])

dnl -----------------------------------
dnl INCLUDES and LIBS for PYTHON

if test "x$SWIG" != "x"; then
AC_ARG_ENABLE(python,
  [AC_HELP_STRING([--enable-python=yes], [python language bindings])],
  [], [enable_python=yes])
if test "x$enable_python" = "xyes"; then
if test `$SWIG -help 2>&1 | $GREP -c '\-python *- Generate'` = 0; then
	PYTHON=
	AC_MSG_WARN([swig does not support -python. The PYTHON packages will not be built])
else
AC_CHECK_PROG(PYTHON,python,python)
if test "x$PYTHON" != "x"; then
  PYTHON_VERSION=`$PYTHON -V 2>&1 | $GREP Python | cut -d ' ' -f 2`
  if test "x$PYTHON_VERSION" = "x"; then
    PYTHON=
  else
    PYTHON_VERSION_MAJOR=`echo $PYTHON_VERSION | cut -d '.' -f 1`
    PYTHON_VERSION_MINOR=`echo $PYTHON_VERSION | cut -d '.' -f 2`
    PYTHON_VERSION_SHORT=$PYTHON_VERSION_MAJOR.$PYTHON_VERSION_MINOR
    if test $PYTHON_VERSION_MAJOR -lt 3; then
      if test $PYTHON_VERSION_MAJOR -lt 2; then
	PYTHON=
      else
        if test $PYTHON_VERSION_MINOR -lt 3; then
	  PYTHON=
        fi
      fi
    fi
  fi
  if test "x$PYTHON" = "x"; then
      AC_MSG_WARN([Can't use Python version $PYTHON_VERSION  The Python packages will not be built])
  else
    PYTHON_PREFIX=`$PYTHON -c "import sys; print sys.prefix"`
    PYTHON_INCLUDES=-I$PYTHON_PREFIX/include/python$PYTHON_VERSION_SHORT
    PYTHON_LIBS="-L$PYTHON_PREFIX/lib$LIBPOSTFIX/python$PYTHON_VERSION_SHORT/config"
    PYTHON_INSTALL_DIR="`$PYTHON $srcdir/config/config_python.py archsitelib`"
    save_CPPFLAGS=$CPPFLAGS
    CPPFLAGS="$CPPFLAGS $PYTHON_INCLUDES"
    AC_CHECK_HEADER(Python.h,,[
      AC_MSG_WARN([Unable to find header Python.h. The Python packages will not be built])
      PYTHON=
      ])
    CPPFLAGS=$save_CPPFLAGS
  fi
fi
AC_SUBST(PYTHON_INCLUDES)
AC_SUBST(PYTHON_LIBS)
AC_SUBST(PYTHON_INSTALL_DIR)
fi
fi
fi
AM_CONDITIONAL(WITH_PYTHON, [test "x$PYTHON" != "x"])

dnl -----------------------------------
dnl INCLUDES and LIBS for PYTHON23

if test "x$SWIG" != "x"; then
AC_ARG_ENABLE(python23,
  [AC_HELP_STRING([--enable-python23=no], [python23 language bindings])],
  [], [enable_python23=no])
if test "x$enable_python23" = "xyes"; then
if test `$SWIG -help 2>&1 | $GREP -c '\-python *- Generate'` = 0; then
	PYTHON23=
	AC_MSG_WARN([swig does not support -python. The PYTHON23 packages will not be built])
else
AC_CHECK_PROG(PYTHON23,python2.3,python2.3)
if test "x$PYTHON23" != "x"; then
  PYTHON23_VERSION=`$PYTHON23 -V 2>&1 | $GREP Python | cut -d ' ' -f 2`
  if test "x$PYTHON23_VERSION" = "x"; then
    PYTHON23=
  else
    PYTHON23_VERSION_MAJOR=`echo $PYTHON23_VERSION | cut -d '.' -f 1`
    PYTHON23_VERSION_MINOR=`echo $PYTHON23_VERSION | cut -d '.' -f 2`
    PYTHON23_VERSION_SHORT=$PYTHON23_VERSION_MAJOR.$PYTHON23_VERSION_MINOR
    if test $PYTHON23_VERSION_MAJOR -lt 3; then
      if test $PYTHON23_VERSION_MAJOR -lt 2; then
	PYTHON23=
      else
        if test $PYTHON23_VERSION_MINOR -lt 3; then
	  PYTHON23=
        fi
      fi
    fi
  fi
  if test "x$PYTHON23" = "x"; then
      AC_MSG_WARN([Can't use Python version $PYTHON23_VERSION  The Python packages will not be built])
  else
    PYTHON23_PREFIX=`$PYTHON23 -c "import sys; print sys.prefix"`
    PYTHON23_INCLUDES=-I$PYTHON23_PREFIX/include/python$PYTHON23_VERSION_SHORT
    PYTHON23_LIBS="-L$PYTHON23_PREFIX/lib$LIBPOSTFIX"
    PYTHON23_INSTALL_DIR="`$PYTHON23 $srcdir/config/config_python.py archsitelib`"
    save_CPPFLAGS=$CPPFLAGS
    CPPFLAGS="$CPPFLAGS $PYTHON23_INCLUDES"
    AC_CHECK_HEADER(Python.h,,[
      AC_MSG_WARN([Unable to find header Python.h. The Python packages will not be built])
      PYTHON23=
      ])
    CPPFLAGS=$save_CPPFLAGS
  fi
fi
AC_SUBST(PYTHON23_INCLUDES)
AC_SUBST(PYTHON23_LIBS)
AC_SUBST(PYTHON23_INSTALL_DIR)
fi
fi
fi
AM_CONDITIONAL(WITH_PYTHON23, [test "x$PYTHON23" != "x"])

dnl -----------------------------------
dnl INCLUDES and LIBS for PYTHON24

if test "x$SWIG" != "x"; then
AC_ARG_ENABLE(python24,
  [AC_HELP_STRING([--enable-python24=no], [python24 language bindings])],
  [], [enable_python24=no])
if test "x$enable_python24" = "xyes"; then
if test `$SWIG -help 2>&1 | $GREP -c '\-python *- Generate'` = 0; then
	PYTHON24=
	AC_MSG_WARN([swig does not support -python. The PYTHON24 packages will not be built])
else
AC_CHECK_PROG(PYTHON24,python2.4,python2.4)
if test "x$PYTHON24" != "x"; then
  PYTHON24_VERSION=`$PYTHON24 -V 2>&1 | $GREP Python | cut -d ' ' -f 2`
  if test "x$PYTHON24_VERSION" = "x"; then
    PYTHON24=
  else
    PYTHON24_VERSION_MAJOR=`echo $PYTHON24_VERSION | cut -d '.' -f 1`
    PYTHON24_VERSION_MINOR=`echo $PYTHON24_VERSION | cut -d '.' -f 2`
    PYTHON24_VERSION_SHORT=$PYTHON24_VERSION_MAJOR.$PYTHON24_VERSION_MINOR
    if test $PYTHON24_VERSION_MAJOR -lt 3; then
      if test $PYTHON24_VERSION_MAJOR -lt 2; then
	PYTHON24=
      else
        if test $PYTHON24_VERSION_MINOR -lt 3; then
	  PYTHON24=
        fi
      fi
    fi
  fi
  if test "x$PYTHON24" = "x"; then
      AC_MSG_WARN([Can't use Python version $PYTHON24_VERSION  The Python packages will not be built])
  else
    PYTHON24_PREFIX=`$PYTHON24 -c "import sys; print sys.prefix"`
    PYTHON24_INCLUDES=-I$PYTHON24_PREFIX/include/python$PYTHON24_VERSION_SHORT
    PYTHON24_LIBS="-L$PYTHON24_PREFIX/lib$LIBPOSTFIX"
    PYTHON24_INSTALL_DIR="`$PYTHON24 $srcdir/config/config_python.py archsitelib`"
    save_CPPFLAGS=$CPPFLAGS
    CPPFLAGS="$CPPFLAGS $PYTHON24_INCLUDES"
    AC_CHECK_HEADER(Python.h,,[
      AC_MSG_WARN([Unable to find header Python.h. The Python packages will not be built])
      PYTHON24=
      ])
    CPPFLAGS=$save_CPPFLAGS
  fi
fi
AC_SUBST(PYTHON24_INCLUDES)
AC_SUBST(PYTHON24_LIBS)
AC_SUBST(PYTHON24_INSTALL_DIR)
fi
fi
fi
AM_CONDITIONAL(WITH_PYTHON24, [test "x$PYTHON24" != "x"])

dnl -----------------------------------
dnl INCLUDES and LIBS for PYTHON25

if test "x$SWIG" != "x"; then
AC_ARG_ENABLE(python25,
  [AC_HELP_STRING([--enable-python25=no], [python25 language bindings])],
  [], [enable_python25=no])
if test "x$enable_python25" = "xyes"; then
if test `$SWIG -help 2>&1 | $GREP -c '\-python *- Generate'` = 0; then
	PYTHON25=
	AC_MSG_WARN([swig does not support -python. The PYTHON25 packages will not be built])
else
AC_CHECK_PROG(PYTHON25,python2.5,python2.5)
if test "x$PYTHON25" != "x"; then
  PYTHON25_VERSION=`$PYTHON25 -V 2>&1 | $GREP Python | cut -d ' ' -f 2`
  if test "x$PYTHON25_VERSION" = "x"; then
    PYTHON25=
  else
    PYTHON25_VERSION_MAJOR=`echo $PYTHON25_VERSION | cut -d '.' -f 1`
    PYTHON25_VERSION_MINOR=`echo $PYTHON25_VERSION | cut -d '.' -f 2`
    PYTHON25_VERSION_SHORT=$PYTHON25_VERSION_MAJOR.$PYTHON25_VERSION_MINOR
    if test $PYTHON25_VERSION_MAJOR -lt 3; then
      if test $PYTHON25_VERSION_MAJOR -lt 2; then
	PYTHON25=
      else
        if test $PYTHON25_VERSION_MINOR -lt 3; then
	  PYTHON25=
        fi
      fi
    fi
  fi
  if test "x$PYTHON25" = "x"; then
      AC_MSG_WARN([Can't use Python version $PYTHON25_VERSION  The Python packages will not be built])
  else
    PYTHON25_PREFIX=`$PYTHON25 -c "import sys; print sys.prefix"`
    PYTHON25_INCLUDES=-I$PYTHON25_PREFIX/include/python$PYTHON25_VERSION_SHORT
    PYTHON25_LIBS="-L$PYTHON25_PREFIX/lib$LIBPOSTFIX"
    PYTHON25_INSTALL_DIR="`$PYTHON25 $srcdir/config/config_python.py archsitelib`"
    save_CPPFLAGS=$CPPFLAGS
    CPPFLAGS="$CPPFLAGS $PYTHON25_INCLUDES"
    AC_CHECK_HEADER(Python.h,,[
      AC_MSG_WARN([Unable to find header Python.h. The Python packages will not be built])
      PYTHON25=
      ])
    CPPFLAGS=$save_CPPFLAGS
  fi
fi
AC_SUBST(PYTHON25_INCLUDES)
AC_SUBST(PYTHON25_LIBS)
AC_SUBST(PYTHON25_INSTALL_DIR)
fi
fi
fi
AM_CONDITIONAL(WITH_PYTHON25, [test "x$PYTHON25" != "x"])

dnl -----------------------------------
dnl INCLUDES and LIBS for R

R=
if test "x$SWIG" != "x"; then
AC_ARG_ENABLE(r,
  [AC_HELP_STRING([--enable-r=yes], [R language bindings])],
  [], [enable_r=yes])
if test "x$enable_r" = "xyes"; then
if test `$SWIG -help 2>&1 | $GREP -c '\-r *- Generate'` = 0; then
	AC_MSG_WARN([swig does not support -r. The R packages will not be built])
else
PKG_CHECK_MODULES(R, [libR],
        [R=1],
        [AC_MSG_WARN(R library not available.)])
fi
fi
fi
AC_SUBST(R_CFLAGS)
AC_SUBST(R_LIBS)
AM_CONDITIONAL(WITH_R, [test "x$R" != "x"])

dnl -----------------------------------
dnl INCLUDES and LIBS for RUBY

if test "x$SWIG" != "x"; then
AC_ARG_ENABLE(ruby,
  [AC_HELP_STRING([--enable-ruby=yes], [ruby language bindings])],
  [], [enable_ruby=yes])
if test "x$enable_ruby" = "xyes"; then
if test `$SWIG -help 2>&1 | $GREP -c '\-ruby *- Generate'` = 0; then
	RUBY=
	AC_MSG_WARN([swig does not support -ruby. The RUBY packages will not be built])
else
AC_CHECK_PROG(RUBY,ruby,ruby)
if test "x$RUBY" != "x"; then
  RUBY_INCLUDES="-I`$RUBY $srcdir/config/config_ruby.rb INCLUDES`"
  RUBY_LIBS="-L`$RUBY $srcdir/config/config_ruby.rb lib` `$RUBY -rrbconfig -e \"puts Config::CONFIG[['LIBRUBYARG_SHARED']]\"`"
  RUBY_INSTALL_DIR="`$RUBY $srcdir/config/config_ruby.rb archsitelib`"
  save_CPPFLAGS=$CPPFLAGS
  CPPFLAGS="$CPPFLAGS $RUBY_INCLUDES"
  AC_CHECK_HEADER(ruby.h,,[
    AC_MSG_WARN([Unable to find header ruby.h. The Ruby packages will not be built])
    RUBY=
    ])
  CPPFLAGS=$save_CPPFLAGS
fi
AC_SUBST(RUBY_INCLUDES)
AC_SUBST(RUBY_LIBS)
AC_SUBST(RUBY_INSTALL_DIR)
fi
fi
fi
AM_CONDITIONAL(WITH_RUBY, [test "x$RUBY" != "x"])

dnl -----------------------------------
dnl INCLUDES and LIBS for TCL

AC_ARG_ENABLE(tcl,
  [AC_HELP_STRING([--enable-tcl=yes], [tcl language bindings])],
  [], [enable_tcl=yes])
if test "x$enable_tcl" = "xyes"; then
# start by assuming we have it
HAVE_TCL=1 
else
HAVE_TCL=0
fi

if test "$HAVE_TCL" = "1"; then
AC_ARG_WITH(tclsh,
    [AC_HELP_STRING([--with-tcl=PROG], [use a specific tclsh])],
    TCLSH=$withval,)

if test "x$TCLSH" = "x"; then
    AC_PATH_PROG(TCLSH,tclsh)
    if test "x$TCLSH" = "x"; then
        AC_PATH_PROG(TCLSH,tclsh8.5)
        if test "x$TCLSH" = "x"; then
            AC_PATH_PROG(TCLSH,tclsh8.4)
            if test "x$TCLSH" = "x"; then
                AC_PATH_PROG(TCLSH,tclsh8.3)
            fi
	fi
    fi
    if test "x$TCLSH" = "x"; then
        AC_MSG_WARN([Unable to find a tclsh. The Tcl packages will not be built])
        HAVE_TCL=0
    fi
fi
fi

if test "$HAVE_TCL" = "1"; then
    TCL_VERSION_FOUND=`echo 'puts [[info tclversion]]' | $TCLSH`

    # can't assume ksh on all architectures
    # TCLSH_EXEC_PREFIX=${TCLSH%%/bin/tclsh.*}
    # so use sed instead
    TCLSH_EXEC_PREFIX=`echo $TCLSH|sed -e 's%/bin/tclsh.*$%%'`

    if test -f ${TCLSH_EXEC_PREFIX}/include/tcl${TCL_VERSION_FOUND}/tcl.h; then
        TCL_INCLUDES="-I${TCLSH_EXEC_PREFIX}/include/tcl${TCL_VERSION_FOUND} -I${TCLSH_EXEC_PREFIX}/include/tcl${TCL_VERSION_FOUND}/generic"
    else
        if test "x${TCLSH_EXEC_PREFIX}" != "x/usr" -a -f ${TCLSH_EXEC_PREFIX}/include/tcl.h; then
            TCL_INCLUDES=-I${TCLSH_EXEC_PREFIX}/include
        fi
    fi
    if test "x$TCL_INCLUDES" != "x" ; then
        AC_MSG_RESULT([using tcl headers from $TCL_INCLUDES])
    fi
    AC_SUBST(TCL_INCLUDES)
    save_CPPFLAGS=$CPPFLAGS
    CPPFLAGS="$CPPFLAGS $TCL_INCLUDES"
    AC_CHECK_HEADER(tcl.h,,[
        AC_MSG_WARN([Unable to find header tcl.h. The Tcl packages will not be built])
        HAVE_TCL=0])
    CPPFLAGS=$save_CPPFLAGS
fi

if test "$HAVE_TCL" = "1"; then
    # Some systems don't have Tcl.  Don't build
    # the Tcl products if we don't have the library.
    if test -f ${TCLSH_EXEC_PREFIX}/lib${LIBPOSTFIX}/tcl${TCL_VERSION_FOUND}/tclConfig.sh; then
        TCLCONFIG=${TCLSH_EXEC_PREFIX}/lib${LIBPOSTFIX}/tcl${TCL_VERSION_FOUND}/tclConfig.sh
    else
        if test -f ${TCLSH_EXEC_PREFIX}/lib${LIBPOSTFIX}/tclConfig.sh; then
            TCLCONFIG=${TCLSH_EXEC_PREFIX}/lib${LIBPOSTFIX}/tclConfig.sh
        else
            if test -f ${TCLSH_EXEC_PREFIX}/lib/tclConfig.sh; then
                TCLCONFIG=${TCLSH_EXEC_PREFIX}/lib/tclConfig.sh
            fi
        fi
    fi
    if test "x$TCLCONFIG" != "x"; then
        AC_MSG_RESULT([using $TCLCONFIG])
        file=${TCLCONFIG}
            . $file
    else
        AC_MSG_WARN([Unable to find tclConfig.sh. The Tcl packages will not be built])
        HAVE_TCL=0
    fi
    TCL_INSTALL_DIR="${TCLSH_EXEC_PREFIX}/lib${LIBPOSTFIX}/tcl${TCL_VERSION_FOUND}"
fi

if test "$HAVE_TCL" = "1"; then
    # TCL STUBS support is required
    if test "x${TCL_SUPPORTS_STUBS}" != "x1"; then
        AC_MSG_WARN([Tcl does not have STUBs support, perhaps it is too old?  The Tcl packages will not be built])
        HAVE_TCL=0
    fi
fi

AC_SUBST(HAVE_TCL)
AC_SUBST(TCL_INSTALL_DIR)

AM_CONDITIONAL(WITH_TCL, [test "x${HAVE_TCL}" = "x1"])

if test "x${TCL_SUPPORTS_STUBS}" = "x1"; then
    TCL_CFLAGS="${TCL_CFLAGS} -DUSE_TCL_STUBS"
fi
#AC_SUBST(TCLSH_EXEC_PREFIX)
AC_SUBST(TCL_VERSION)
AC_SUBST(TCL_MAJOR_VERSION)
AC_SUBST(TCL_MINOR_VERSION)
AC_SUBST(TCL_PATCH_LEVEL)
#AC_SUBST(TCL_CC)
#AC_SUBST(TCL_CXX)
AC_SUBST(TCL_CFLAGS)
#AC_SUBST(TCL_CXXFLAGS)
AC_SUBST(TCL_DEFS)
#AC_SUBST(TCL_DBGX)
#AC_SUBST(TCL_CFLAGS_DEBUG)
#AC_SUBST(TCL_CFLAGS_OPTIMIZE)
#AC_SUBST(TCL_SHARED_BUILD)
#AC_SUBST(TCL_LIB_FILE)
#AC_SUBST(TCL_NEEDS_EXP_FILE)
#AC_SUBST(TCL_EXPORT_FILE_SUFFIX)
#AC_SUBST(TCL_NEEDS_EXP_FILE)
#AC_SUBST(TCL_EXPORT_FILE_SUFFIX)

# TCL modules use stubs and don't need any other tcl libraries
TCL_LIBS=
AC_SUBST(TCL_LIBS)

#AC_SUBST(TCL_PREFIX)
#AC_SUBST(TCL_EXEC_PREFIX)
#AC_SUBST(TCL_SHLIB_CFLAGS)
#AC_SUBST(TCL_SHLIB_CXXFLAGS)
#AC_SUBST(TCL_EXTRA_CFLAGS)
#AC_SUBST(TCL_SHLIB_LD)
#AC_SUBST(TCL_SHLIB_LD_LIBS)
AC_SUBST(TCL_SHLIB_SUFFIX)
#AC_SUBST(TCL_DL_LIBS)
#AC_SUBST(TCL_LD_FLAGS)
#AC_SUBST(TCL_LD_SEARCH_FLAGS)
#AC_SUBST(TCL_COMPAT_OBJS)
#AC_SUBST(TCL_RANLIB)
#AC_SUBST(TCL_LIB_FLAG)
#AC_SUBST(TCL_BUILD_LIB_SPEC)
#AC_SUBST(TCL_LIB_SPEC)
#AC_SUBST(TCL_LIB_VERSIONS_OK)
#AC_SUBST(TCL_SHARED_LIB_SUFFIX)
#AC_SUBST(TCL_UNSHARED_LIB_SUFFIX)
AC_SUBST(TCL_SRC_DIR)
#AC_SUBST(TCL_PACKAGE_PATH)
#AC_SUBST(TCL_SUPPORTS_STUBS)
#AC_SUBST(TCL_STUB_LIB_FILE)
#AC_SUBST(TCL_STUB_LIB_FLAG)
#AC_SUBST(TCL_BUILD_STUB_LIB_SPEC)
AC_SUBST(TCL_STUB_LIB_SPEC)
#AC_SUBST(TCL_BUILD_STUB_LIB_PATH)
#AC_SUBST(TCL_STUB_LIB_PATH)

dnl -----------------------------------
dnl INCLUDES and LIBS for TK

# start by assuming we have it, if we have tcl
if test "$HAVE_TCL" = "0"; then
    AC_MSG_WARN([No Tcl. The Tk packages will not be built either])
fi

HAVE_TK=$HAVE_TCL
                                                                                
if test "$HAVE_TK" = "1"; then
    AC_ARG_WITH(wish,
        [AC_HELP_STRING([--with-wish=PROG], [use a specific wish])],
        WISH=$withval,)
    if test "x$WISH" = "x"; then
        AC_PATH_PROG(WISH,wish)
        if test "x$WISH" = "x"; then
            AC_PATH_PROG(WISH,wish8.5)
            if test "x$WISH" = "x"; then
    	        AC_PATH_PROG(WISH,wish8.4)
    	        if test "x$WISH" = "x"; then
                    AC_PATH_PROG(WISH,wish8.3)
                fi
            fi
	fi
    fi
    if test "x$WISH" = "x"; then
        AC_MSG_WARN([Unable to find a wish. The Tk packages will not be built])
        HAVE_TK=0
    fi
fi

if test "$HAVE_TK" = "1"; then
    #TK_VERSION_FOUND=`echo 'puts [[info tkversion]]' | $WISH`
    #
    # No such var as tkversion.  Use tclversion instead.
    TK_VERSION_FOUND=${TCL_VERSION_FOUND}
    
    # can't assume ksh on all architectures
    # WISH_EXEC_PREFIX=${WISH%%/bin/tclsh.*}
    # so use sed instead
    WISH_EXEC_PREFIX=`echo $WISH|sed -e 's%/bin/wish.*$%%'`

    if test -f ${WISH_EXEC_PREFIX}/include/tk${TK_VERSION_FOUND}/tk.h; then
	TK_INCLUDES="-I${WISH_EXEC_PREFIX}/include/tk${TK_VERSION_FOUND} -I${WISH_EXEC_PREFIX}/include/tk${TK_VERSION_FOUND}/generic -I${WISH_EXEC_PREFIX}/include/tk${TK_VERSION_FOUND}/unix"
    else
        if test "x${WISH_EXEC_PREFIX}" != "x/usr" -a -f ${WISH_EXEC_PREFIX}/include/tk.h; then
	    TK_INCLUDES=-I${WISH_EXEC_PREFIX}/include
        fi
    fi
    if test "x$TK_INCLUDES" != "x" ; then
	AC_MSG_RESULT([using tk headers from $TK_INCLUDES])
    fi
    AC_SUBST(TK_INCLUDES)
    save_CPPFLAGS=$CPPFLAGS
    CPPFLAGS="$CPPFLAGS $TCL_INCLUDES $TK_INCLUDES $X_CFLAGS"
    AC_CHECK_HEADER(tk.h,,[
        AC_MSG_WARN([Unable to find header tk.h. The Tk packages will not be built])
        HAVE_TK=0])
    AC_CHECK_HEADERS(tk.h tkInt.h)
    CPPFLAGS=$save_CPPFLAGS
fi

if test "$HAVE_TK" = "1"; then
    # Some systems don't have Tcl.  Don't build
    # Some systems have Tcl, but not Tk.  Don't build
    # the Tk products if we don't have the library.
    if test -f ${WISH_EXEC_PREFIX}/lib${LIBPOSTFIX}/tk${TK_VERSION_FOUND}/tkConfig.sh; then
        TKCONFIG=${WISH_EXEC_PREFIX}/lib${LIBPOSTFIX}/tk${TK_VERSION_FOUND}/tkConfig.sh
    else
        if test -f ${WISH_EXEC_PREFIX}/lib${LIBPOSTFIX}/tkConfig.sh; then
	    TKCONFIG=${WISH_EXEC_PREFIX}/lib${LIBPOSTFIX}/tkConfig.sh
	else
            if test -f ${WISH_EXEC_PREFIX}/lib/tkConfig.sh; then
	        TKCONFIG=${WISH_EXEC_PREFIX}/lib/tkConfig.sh
            fi
        fi
    fi
    if test "x$TKCONFIG" != "x"; then
        AC_MSG_RESULT([using $TKCONFIG])
        file=${TKCONFIG}
        . $file
    else
        AC_MSG_WARN([Unable to find tkConfig.sh. The Tk packages will not be built])
        HAVE_TK=0
    fi
fi
                                                                                
if test "$HAVE_TK" = "1"; then
    TK_SUBDIRS="tkstubs tkspline"
else
    TK_SUBDIRS=""
fi
AC_SUBST(HAVE_TK)
AM_CONDITIONAL(WITH_TK, [test "x${HAVE_TK}" = "x1"])

# NB. No TK_SUPPORTS_STUBS set by tkConfig.sh
if test "${TCL_SUPPORTS_STUBS}" = "1"; then
    TK_CFLAGS="${TK_CFLAGS} -DUSE_TK_STUBS"
fi

AC_SUBST(TK_VERSION)
AC_SUBST(TK_MAJOR_VERSION)
AC_SUBST(TK_MINOR_VERSION)
AC_SUBST(TK_PATCH_LEVEL)
AC_SUBST(TK_DEFS)
#AC_SUBST(TK_SHARED_BUILD)
#AC_SUBST(TK_LIB_FILE)
#AC_SUBST(TK_LIBS)
#AC_SUBST(TK_PREFIX)
#AC_SUBST(TK_EXEC_PREFIX)
AC_SUBST(TK_XINCLUDES)
#AC_SUBST(TK_XLIBSW)
#AC_SUBST(TK_BUILD_LIB_SPEC)
AC_SUBST(TK_LIB_SPEC)
AC_SUBST(TK_SRC_DIR)
AC_SUBST(TK_CFLAGS)
#AC_SUBST(TK_SUPPORTS_STUBS)
#AC_SUBST(TK_STUB_LIB_FILE)
#AC_SUBST(TK_STUB_LIB_FLAG)
#AC_SUBST(TK_BUILD_STUB_LIB_SPEC)
AC_SUBST(TK_STUB_LIB_SPEC)
#AC_SUBST(TK_BUILD_STUB_LIB_PATH)
#AC_SUBST(TK_STUB_LIB_PATH)

dnl -----------------------------------
dnl Support for generic "extra" search paths for includes and libraries

AC_ARG_WITH(extraincludedir,
  [AC_HELP_STRING([--with-extraincludedir=DIR], [use extra includes from DIR])],
  [CPPFLAGS="$CPPFLAGS -I$withval"])

AC_ARG_WITH(extralibdir,
  [AC_HELP_STRING([--with-extralibdir=DIR], [use extra libraries from DIR])],
  [LDFLAGS="$LDFLAGS -L$withval"])

dnl -----------------------------------
dnl ...borrow test for -lsocket -lnsl from tk8.3.3/unix/configure.in

save_LIBS=$LIBS
LIBS=
tk_checkBoth=0
AC_CHECK_FUNC(connect, tk_checkSocket=0, tk_checkSocket=1)
if test "$tk_checkSocket" = 1; then
  AC_CHECK_LIB(socket, main, LIBS="$LIBS -lsocket", tk_checkBoth=1)
fi
if test "$tk_checkBoth" = 1; then
  tk_oldLibs=$LIBS
  LIBS="$LIBS -lsocket -lnsl"
  AC_CHECK_FUNC(accept, tk_checkNsl=0, [LIBS=$tk_oldLibs])
fi  
AC_CHECK_FUNC(gethostbyname, , [AC_CHECK_LIB(nsl, main, [LIBS="$LIBS -lnsl"])])
SOCKET_LIBS=$LIBS
AC_SUBST(SOCKET_LIBS)
LIBS=$save_LIBS

dnl -----------------------------------
dnl INCLUDES and LIBS for EXPAT

save_CPPFLAGS=$CPPFLAGS
save_LDFLAGS=$LDFLAGS
AC_PATH_PROG(EXPAT_CONFIG,expat-config)
if test -n "$EXPAT_CONFIG"; then
	EXPAT_INCLUDES=`$EXPAT_CONFIG --cflags`
	EXPAT_PREFIX=`$EXPAT_CONFIG --prefix`
	if test "${EXPAT_PREFIX}/lib" != "/usr/lib" ; then
		EXPAT_LIBS="-L${EXPAT_PREFIX}/lib"
	fi
else         
	EXPAT_INCLUDES=""
	EXPAT_LIBS=""
fi
AC_ARG_WITH(expatincludedir,
  [AC_HELP_STRING([--with-expatincludedir=DIR], [use EXPAT includes from DIR])],
  [EXPAT_INCLUDES="-I$withval"])
CPPFLAGS="$CPPFLAGS $EXPAT_INCLUDES"
EXPAT_LIBDIR="/usr/lib$LIBPOSTFIX"
AC_ARG_WITH(expatlibdir,
  [AC_HELP_STRING([--with-expatlibdir=DIR], [use EXPAT libraries from DIR])],
  [EXPAT_LIBDIR="$withval"])
if test "$EXPAT_LIBDIR" != "/usr/lib$LIBPOSTFIX"; then
	EXPAT_LIBS="-L$EXPAT_LIBDIR"
	LDFLAGS="$LDFLAGS $EXPAT_LIBS"
fi
if test -f "$EXPAT_LIBDIR/libexpat.la"; then
	EXPAT_LIBS="$EXPAT_LIBDIR/libexpat.la"
fi
AC_CHECK_HEADER(expat.h,
	[AC_CHECK_LIB(expat,main,
		[EXPAT_LIBS="$EXPAT_LIBS -lexpat"
		AC_DEFINE_UNQUOTED(HAVE_LIBEXPAT,1,
			[Define if you have the EXPAT library])],
		[AC_MSG_WARN(Optional expat library not available)
		EXPAT_INCLUDES="" EXPAT_LIBS=""])],
	[AC_MSG_WARN(Optional expat library not available - no expat.h)])
LDFLAGS=$save_LDFLAGS
CPPFLAGS=$save_CPPFLAGS
AC_SUBST(EXPAT_INCLUDES)
AC_SUBST(EXPAT_LIBS)

dnl -----------------------------------
dnl INCLUDES and LIBS for DEVIL

AC_ARG_WITH(devil,
  [AC_HELP_STRING([--with-devil=yes], [DevIL plugin])],
  [],[with_devil=yes])
AC_ARG_WITH(devilincludedir,
  [AC_HELP_STRING([--with-devilincludedir=DIR], [use DevIL includes from DIR])],
  [DEVIL_INCLUDES="-I$withval"])
AC_ARG_WITH(devillibdir,
  [AC_HELP_STRING([--with-devillibdir=DIR], [use DevIL libraries from DIR])],
  [DEVIL_LIBS="-L$withval"])

if test "x$with_devil" = "xyes"; then
  save_CPPFLAGS=$CPPFLAGS
  save_LDFLAGS=$LDFLAGS
  CPPFLAGS="$CPPFLAGS $DEVIL_INCLUDES"
  LDFLAGS="$LDFLAGS $DEVIL_LIBS"
  AC_CHECK_HEADERS([IL/il.h],
    [],
    [with_devil=no
     AC_MSG_WARN(Optional DevIL library not available - missing headers)],
    [])
  AC_CHECK_LIB(IL,main,
    [DEVIL_LIBS="$DEVIL_LIBS -lIL -lILU"],
    [with_devil=no
     AC_MSG_WARN(Optional DevIL library not available)],
    [])
  LDFLAGS=$save_LDFLAGS
  CPPFLAGS=$save_CPPFLAGS
fi

if test "x$with_devil" = "xyes"; then
     AC_DEFINE_UNQUOTED(HAVE_DEVIL,1,[Define if you have the DevIL library])
fi
AC_SUBST(HAVE_DEVIL)
AC_SUBST(DEVIL_INCLUDES)
AC_SUBST(DEVIL_LIBS)
AM_CONDITIONAL(WITH_DEVIL, [test "x$with_devil" = "xyes"])

dnl -----------------------------------
dnl INCLUDES and LIBS for XPM
#
# we don't want an X11 dependency in libgd
# so we set HAVE_LIBXPMFORLEFTY instead of the usual HAVE_LIBXPM
#

AC_ARG_WITH(Xpmincludedir,
  [AC_HELP_STRING([--with-Xpmincludedir=DIR], [use Xpm includes from DIR])],
  [XPM_INCLUDES="-I$withval"])
AC_ARG_WITH(Xpmlibdir,
  [AC_HELP_STRING([--with-Xpmlibdir=DIR], [use Xpm libraries from DIR])],
  [XPM_LIBS="-L$withval"])

save_CPPFLAGS=$CPPFLAGS
save_LDFLAGS=$LDFLAGS
CPPFLAGS="$CPPFLAGS $XPM_INCLUDES $X_CFLAGS"
LDFLAGS="$LDFLAGS $XPM_LIBS $X_LIBS $X_EXTRA_LIBS $X_PRE_LIBS $SOCKET_LIBS"
AC_CHECK_LIB(Xpm,main,
  [XPM_LIBS="$XPM_LIBS -lXpm"
   AC_DEFINE_UNQUOTED(HAVE_LIBXPMFORLEFTY,1,[Define if you have the XPM library])],
   AC_MSG_WARN(Optional Xpm library not available), [-lXmu -lXt -lX11 -lXext $SOCKET_LIBS])
LDFLAGS=$save_LDFLAGS
CPPFLAGS=$save_CPPFLAGS
AC_SUBST(XPM_INCLUDES)
AC_SUBST(XPM_LIBS)

dnl -----------------------------------
dnl INCLUDES and LIBS for XAW

AC_ARG_WITH(Xawincludedir,
  [AC_HELP_STRING([--with-Xawincludedir=DIR], [use Xaw includes from DIR])],
  [XAW_INCLUDES="-I$withval"])
AC_ARG_WITH(Xawlibdir,
  [AC_HELP_STRING([--with-Xawlibdir=DIR], [use Xaw libraries from DIR])],
  [XAW_LIBS="-L$withval"])
save_CPPFLAGS=$CPPFLAGS
save_LDFLAGS=$LDFLAGS
CPPFLAGS="$CPPFLAGS $XAW_INCLUDES $X_CFLAGS"
LDFLAGS="$LDFLAGS $X_LIBS $XAW_LIBS $XPM_LIBS $X_EXTRA_LIBS $X_PRE_LIBS $SOCKET_LIBS"
AC_CHECK_HEADERS([X11/Intrinsic.h])
AC_CHECK_HEADERS([X11/Xaw/Text.h],
		 [],
		 [with_xaw=no
		  AC_MSG_WARN(Optional Xaw library not available - missing headers)],
		 [#if HAVE_X11_INTRINSIC_H
		  # include <X11/Intrinsic.h>
		  # endif
		  ])
if test "x$with_xaw" != "xno"; then
AC_CHECK_LIB(Xaw,main,
	     [XAW_LIBS="$X_LIBS $XAW_LIBS -lXaw"],
	     [with_xaw=no
	      AC_MSG_WARN(Optional Xaw library not available)],
	      [$X_LIBS -lXmu -lXt -lX11 -lXext $SOCKET_LIBS])
fi
LDFLAGS=$save_LDFLAGS
CPPFLAGS=$save_CPPFLAGS
AC_SUBST(XAW_INCLUDES)
AC_SUBST(XAW_LIBS)
AM_CONDITIONAL(WITH_XAW, [test "x$with_xaw" != "xno"])
if test "x$with_xaw" = "xno" && "x$no_x" != xyes; then
	AC_MSG_WARN(Lefty cannot be built)
fi

dnl -----------------------------------
dnl INCLUDES and LIBS for Z

AC_ARG_WITH(zincludedir,
  [AC_HELP_STRING([--with-zincludedir=DIR], [use Z includes from DIR])],
  [Z_INCLUDES="-I$withval"])
AC_ARG_WITH(zlibdir,
  [AC_HELP_STRING([--with-zlibdir=DIR], [use Z libraries from DIR])],
  [Z_LIBS="-L$withval"])

save_CPPFLAGS=$CPPFLAGS
save_LDFLAGS=$LDFLAGS
CPPFLAGS="$CPPFLAGS $Z_INCLUDES"
LDFLAGS="$LDFLAGS $Z_LIBS"
AC_CHECK_HEADER(zlib.h,
	[AC_CHECK_LIB(z,main,
		[Z_LIBS="$Z_LIBS -lz"
		AC_DEFINE_UNQUOTED(HAVE_LIBZ,1,[Define if you have the Z library])],
			AC_MSG_WARN(Optional z library not available))],
	AC_MSG_WARN(Optional z library not available - no zlib.h))
LDFLAGS=$save_LDFLAGS
CPPFLAGS=$save_CPPFLAGS
AC_SUBST(Z_INCLUDES)
AC_SUBST(Z_LIBS)

dnl -----------------------------------
dnl SMYRNA 

AC_ARG_WITH(smyrna,
  [AC_HELP_STRING([--with-smyrna=no], [SMYRNA large graph viewer])],
  [], [with_smyrna=no])

if test "x$with_smyrna" = "xyes"; then
	AC_DEFINE_UNQUOTED(SMYRNA,1,[Define if you want SMYRNA])
else
	AC_MSG_WARN(Optional SMYRNA is disabled)
fi
AM_CONDITIONAL(WITH_SMYRNA, [test "x$with_smyrna" = "xyes"])

dnl -----------------------------------
dnl DIGCOLA 

AC_ARG_WITH(digcola,
  [AC_HELP_STRING([--with-digcola=yes], [DIGCOLA features in neato layout engine])],
  [], [with_digcola=yes])

if test "x$with_digcola" = "xyes"; then
	AC_DEFINE_UNQUOTED(DIGCOLA,1,[Define if you want DIGCOLA])
else
	AC_MSG_WARN(Optional DIGCOLA is disabled)
fi

dnl -----------------------------------
dnl IPSEPCOLA   /* disabled by default due to C++ issues on some platforms */
AC_ARG_WITH(ipsepcola,
  [AC_HELP_STRING([--with-ipsepcola=no],
	[IPSEPCOLA features in neato layout engine])],
  [], [with_ipsepcola=no])

### unless digcola was not enabled since ipsecola depends on it
if test "x$with_digcola" = "xno"; then
	AC_MSG_WARN(Optional IPSEPCOLA is disabled - depends on DIGCOLA)
	with_ipsepcola=no
fi

if test "x$with_ipsepcola" = "xyes"; then
	AC_DEFINE_UNQUOTED(IPSEPCOLA,1,[Define if you want IPSEPCOLA])
	IPSEPCOLA_INCLUDES='-I$(top_srcdir)/lib/vpsc'
	IPSEPCOLA_LIBS='$(top_builddir)/lib/vpsc/libvpsc_C.la -lstdc++'
AC_SUBST(IPSEPCOLA_INCLUDES)
AC_SUBST(IPSEPCOLA_LIBS)
fi
AM_CONDITIONAL(WITH_IPSEPCOLA, [test "x$with_ipsepcola" = "xyes"])

dnl -----------------------------------
dnl INCLUDES and LIBS for SVG
                                                                                
AC_ARG_WITH(rsvg,
  [AC_HELP_STRING([--with-rsvg=yes], [rsvg library])],
  [], [with_rsvg=yes])

if test "x$with_rsvg" = "xyes"; then
PKG_CHECK_MODULES(RSVG, [librsvg-2.0],
	[HAVE_RSVG=1
	AC_DEFINE_UNQUOTED(HAVE_RSVG,1,
		[Define if you have the rsvg library])],
	[AC_MSG_WARN(rsvg library not available)])
AC_SUBST(RSVG_CFLAGS)
AC_SUBST(RSVG_LIBS)
fi
AM_CONDITIONAL(WITH_RSVG, [test "x$HAVE_RSVG" != "x"])

dnl -----------------------------------
dnl INCLUDES and LIBS for PANGO, CAIRO et.al.
                                                                                
AC_ARG_WITH(pangocairo,
  [AC_HELP_STRING([--with-pangocairo=yes], [pangocairo library])],
  [], [with_pangocairo=yes])

if test "x$with_pangocairo" = "xyes"; then
# require at least the version available on centos5 (fc6 has 1.14.10)
PKG_CHECK_MODULES(PANGOCAIRO, [pangocairo >= 1.14.9],
	[HAVE_PANGOCAIRO=1
	AC_DEFINE_UNQUOTED(HAVE_PANGOCAIRO,1,
		[Define if you have the pango library with cairo support])],
	[AC_MSG_WARN(pangocairo library not available)])
AC_SUBST(PANGOCAIRO_CFLAGS)
AC_SUBST(PANGOCAIRO_LIBS)
fi
AM_CONDITIONAL(WITH_PANGOCAIRO, [test "x$HAVE_PANGOCAIRO" != "x"])

dnl -----------------------------------
dnl INCLUDES and LIBS for FREETYPE2
                                                                                
AC_ARG_WITH(freetype2,
  [AC_HELP_STRING([--with-freetype2=yes], [freetype2 library])],
  [], [with_freetype2=yes])

if test "x$with_freetype2" = "xyes"; then
PKG_CHECK_MODULES(FREETYPE2, [freetype2],
	  [HAVE_FREETYPE2=1],
	  [AC_MSG_WARN(pkg-config did not find a freetype2.pc.  Looking for freetype-config.)])

# pkgconfig for freetype2 did not exist in redhat[78]
if test "x$HAVE_FREETYPE2" != "x1"; then
    if test "x$FREETYPE_DIR" = "xyes"; then
	AC_PATH_PROG(FREETYPE_CONFIG,freetype-config)
    else
	AC_PATH_PROG(FREETYPE_CONFIG,freetype-config,,[$FREETYPE_DIR/bin:$PATH])
    fi

    if test -n "$FREETYPE_CONFIG"; then
        if test "x$FREETYPE_DIR" != "xyes"; then
	    FREETYPE2_CFLAGS="`$FREETYPE_CONFIG --cflags` -I$FREETYPE_DIR/include"
	else
	    FREETYPE2_CFLAGS=`$FREETYPE_CONFIG --cflags`
	fi
	FREETYPE2_LIBS=`$FREETYPE_CONFIG --libs`
	HAVE_FREETYPE2=1
    fi
fi

if test "x$HAVE_FREETYPE2" != "x"; then
	AC_DEFINE_UNQUOTED(HAVE_FREETYPE2,1,[Define if you have the freetype2 library])
else
	AC_MSG_WARN(freetype2 library not available)
fi
fi
AC_SUBST(FREETYPE2_CFLAGS)
AC_SUBST(FREETYPE2_LIBS)
AM_CONDITIONAL(WITH_FREETYPE2, [test "x$HAVE_FREETYPE2" != "x"])

dnl -----------------------------------
dnl INCLUDES and LIBS for FONTCONFIG
                                                                                
AC_ARG_WITH(fontconfig,
  [AC_HELP_STRING([--with-fontconfig=yes], [use fontconfig library])],
  [], [with_fontconfig=yes])

if test "x$with_fontconfig" = "xyes"; then
PKG_CHECK_MODULES(FONTCONFIG, [fontconfig],
	  [HAVE_FONTCONFIG=1],
	  [AC_MSG_WARN(pkg-config did not find a fontconfig.pc.  Looking for fontconfig-config.)])

# just in case pkgconfig for fontconfig does not exist
if test "x$HAVE_FONTCONFIG" != "x1"; then
    if test "x$FONTCONFIG_DIR" = "xyes"; then
	AC_PATH_PROG(FONTCONFIG_CONFIG,fontconfig-config)
    else
	AC_PATH_PROG(FONTCONFIG_CONFIG,fontconfig-config,,[$FONTCONFIG_DIR/bin:$PATH])
    fi

    if test -n "$FONTCONFIG_CONFIG"; then
        if test "x$FONTCONFIG_DIR" != "xyes"; then
	    FONTCONFIG_CFLAGS="`$FONTCONFIG_CONFIG --cflags` -I$FONTCONFIG_DIR/include"
	else
	    FONTCONFIG_CFLAGS=`$FONTCONFIG_CONFIG --cflags`
	fi
	FONTCONFIG_LIBS=`$FONTCONFIG_CONFIG --libs`
	HAVE_FONTCONFIG=1
    fi
fi

if test "x$HAVE_FONTCONFIG" != "x"; then
	AC_DEFINE_UNQUOTED(HAVE_FONTCONFIG,1,[Define if you have the fontconfig library])
else
	AC_MSG_WARN(fontconfig library not available)
fi
fi
AC_SUBST(FONTCONFIG_CFLAGS)
AC_SUBST(FONTCONFIG_LIBS)
AM_CONDITIONAL(WITH_FONTCONFIG, [test "x$HAVE_FONTCONFIG" != "x"])

dnl -----------------------------------
dnl INCLUDES and LIBS for GDK_PIXBUF.

AC_ARG_WITH(gdk-pixbuf,
  [AC_HELP_STRING([--with-gdk-pixbuf=yes], [gdk-pixbuf library])],
  [], [with_gdk_pixbuf=yes])

if test "x$with_gdk_pixbuf" = "xyes"; then
PKG_CHECK_MODULES(GDK_PIXBUF, [gdk-pixbuf-2.0],
        [HAVE_GDK_PIXBUF=1
        AC_DEFINE_UNQUOTED(HAVE_GDK_PIXBUF,1,
                [Define if you have the gdk-pixbuf library])],
        [AC_MSG_WARN(gdk-pixbuf library not available.)])
fi
AC_SUBST(GDK_PIXBUF_CFLAGS)
AC_SUBST(GDK_PIXBUF_LIBS)
AM_CONDITIONAL(WITH_GDK_PIXBUF, [test "x$HAVE_GDK_PIXBUF" != "x"])

dnl -----------------------------------
dnl INCLUDES and LIBS for GTK.

AC_ARG_WITH(gtk,
  [AC_HELP_STRING([--with-gtk=yes], [gtk+ library])],
  [], [with_gtk=yes])

if test "x$with_gtk" = "xyes"; then
PKG_CHECK_MODULES(GTK, [gtk+-2.0 >= 2.7.0],
        [HAVE_GTK=1
        AC_DEFINE_UNQUOTED(HAVE_GTK,1,
                [Define if you have the gtk+ library])],
        [AC_MSG_WARN(gtk+ library not available. desktop features will not be built)])
fi
AC_SUBST(GTK_CFLAGS)
AC_SUBST(GTK_LIBS)
AM_CONDITIONAL(WITH_GTK, [test "x$HAVE_GTK" != "x"])

dnl -----------------------------------
dnl INCLUDES and LIBS for GTKGL.

AC_ARG_WITH(gtkgl,
  [AC_HELP_STRING([--with-gtkgl=yes], [gtkgl library])],
  [], [with_gtkgl=yes])

if test "x$with_gtkgl" = "xyes"; then
PKG_CHECK_MODULES(GTKGL, [gtkgl-2.0],
        [HAVE_GTKGL=1
        AC_DEFINE_UNQUOTED(HAVE_GTKGL,1,
                [Define if you have the gtkgl library])],
        [AC_MSG_WARN(gtkgl library not available. desktop features will not be built)])
fi
AC_SUBST(GTKGL_CFLAGS)
AC_SUBST(GTKGL_LIBS)
AM_CONDITIONAL(WITH_GTKGL, [test "x$HAVE_GTKGL" != "x"])

dnl -----------------------------------
dnl INCLUDES and LIBS for GTKGLEXT.

AC_ARG_WITH(gtkglext,
  [AC_HELP_STRING([--with-gtkglext=yes], [gtkglext library])],
  [], [with_gtkglext=yes])

if test "x$with_gtkglext" = "xyes"; then
PKG_CHECK_MODULES(GTKGLEXT, [gtkglext-1.0],
        [HAVE_GTKGLEXT=1
        AC_DEFINE_UNQUOTED(HAVE_GTKGLEXT,1,
                [Define if you have the gtkglext library])],
        [AC_MSG_WARN(gtkglext library not available. desktop features will not be built)])
fi
AC_SUBST(GTKGLEXT_CFLAGS)
AC_SUBST(GTKGLEXT_LIBS)
AM_CONDITIONAL(WITH_GTKGLEXT, [test "x$HAVE_GTKGLEXT" != "x"])

dnl -----------------------------------
dnl INCLUDES and LIBS for GLADE.

AC_ARG_WITH(glade,
  [AC_HELP_STRING([--with-glade=yes], [glade library])],
  [], [with_glade=yes])

if test "x$with_glade" = "xyes"; then
PKG_CHECK_MODULES(GLADE, [libglade-2.0],
        [HAVE_GLADE=1
        AC_DEFINE_UNQUOTED(HAVE_GLADE,1,
                [Define if you have the glade library])],
        [AC_MSG_WARN(glade library not available. desktop features will not be built)])
fi
AC_SUBST(GLADE_CFLAGS)
AC_SUBST(GLADE_LIBS)
AM_CONDITIONAL(WITH_GLADE, [test "x$HAVE_GLADE" != "x"])


dnl -----------------------------------
dnl INCLUDES and LIBS for GNOMEUI.

AC_ARG_WITH(gnomeui,
  [AC_HELP_STRING([--with-gnomeui=yes], [gnomeui library])],
  [], [with_gnomeui=yes])

if test "x$with_gnomeui" = "xyes"; then
PKG_CHECK_MODULES(GNOMEUI, [libgnomeui-2.0],
        [HAVE_GNOMEUI=1
        AC_DEFINE_UNQUOTED(HAVE_GNOMEUI,1,
                [Define if you have the gnomeui library])],
        [AC_MSG_WARN(gnomeui library not available. using alternate code for firefox interaction)])
fi
AC_SUBST(GNOMEUI_CFLAGS)
AC_SUBST(GNOMEUI_LIBS)
AM_CONDITIONAL(WITH_GNOMEUI, [test "x$HAVE_GNOMEUI" != "x"])

dnl -----------------------------------
dnl INCLUDES and LIBS for GETTEXT.

GETTEXT_PACKAGE=glade-2.0
AC_SUBST(GETTEXT_PACKAGE)
AC_DEFINE_UNQUOTED(GETTEXT_PACKAGE,"$GETTEXT_PACKAGE", [Gettext package.])

ALL_LINGUAS=""
AM_GLIB_GNU_GETTEXT

dnl -----------------------------------
dnl INCLUDES and LIBS for MING (SWF renderer)

AC_ARG_WITH(ming,
  [AC_HELP_STRING([--with-ming=yes], [ming library (plugin for -Tswf)])],
  [], [with_ming=yes])

if test "x$with_ming" = "xyes"; then
PKG_CHECK_MODULES(MING, [libming >= 0.4],
	[HAVE_MING=1
	AC_DEFINE_UNQUOTED(HAVE_MING,1,
		[Define if you have the ming library for SWF support])],
	[AC_MSG_WARN(ming library not available)])
fi
AC_SUBST(MING_CFLAGS)
AC_SUBST(MING_LIBS)
AM_CONDITIONAL(WITH_MING, [test "x$HAVE_MING" != "x"])

dnl -----------------------------------
dnl INCLUDES and LIBS for QUARTZ 

AC_ARG_WITH(quartz,
  [AC_HELP_STRING([--with-quartz=no], [Quartz framework (Mac OS X)])],
  [], [with_quartz=no])

if test "x$with_quartz" = "xyes"; then

if test -d "/System/Library/Frameworks/ApplicationServices.framework"; then
HAVE_QUARTZ=1
AC_DEFINE_UNQUOTED(HAVE_QUARTZ,1,[Define if you have the Quartz framework for Mac OS X])
QUARTZ_CFLAGS=''
QUARTZ_LIBS='-framework ApplicationServices'
fi

fi
AC_SUBST(QUARTZ_CFLAGS)
AC_SUBST(QUARTZ_LIBS)
AM_CONDITIONAL(WITH_QUARTZ, [test "x$HAVE_QUARTZ" != "x"])

dnl -----------------------------------
dnl INCLUDES and LIBS for GD
                                                                                
AC_ARG_WITH(libgd,
  [AC_HELP_STRING([--with-libgd=yes], [use gd library])],
  [], [with_libgd=yes])

AC_ARG_WITH(mylibgd,
  [AC_HELP_STRING([--with-mylibgd=no],
	[use internal gd library in preference to any installed libgd])],
  [], [with_mylibgd=no])

if test "x$with_mylibgd" = "xyes"; then
	with_libgd=yes
fi

if test "x$with_libgd" = "xyes"; then
if test "x$with_mylibgd" = "xno"; then
    AC_PATH_PROG(GDLIB_CONFIG,gdlib-config)
    if test -n "$GDLIB_CONFIG"; then
        GD_INCLUDES=`$GDLIB_CONFIG --includes`
        GD_LDFLAGS=`$GDLIB_CONFIG --ldflags`
        GD_LIBS=`$GDLIB_CONFIG --libs`
        GD_MAJORVERSION=`$GDLIB_CONFIG --majorversion`
        GD_MINORVERSION=`$GDLIB_CONFIG --minorversion`
        GD_REVISION=`$GDLIB_CONFIG --revision`
        for f in `$GDLIB_CONFIG --features` ; do
	    if test "$f" = "GD_PNG"; then
                AC_DEFINE_UNQUOTED(HAVE_GD_PNG,1,[Define if the GD library supports PNG])
	    fi
	    if test "$f" = "GD_JPEG"; then
                AC_DEFINE_UNQUOTED(HAVE_GD_JPEG,1,[Define if the GD library supports JPEG])
	    fi
	    if test "$f" = "GD_XPM"; then
                AC_DEFINE_UNQUOTED(HAVE_GD_XPM,1,[Define if the GD library supports XPM])
	    fi
	    if test "$f" = "GD_FONTCONFIG"; then
                AC_DEFINE_UNQUOTED(HAVE_GD_FONTCONFIG,1,[Define if the GD library supports FONTCONFIG])
	    fi
	    if test "$f" = "GD_FREETYPE"; then
                AC_DEFINE_UNQUOTED(HAVE_GD_FREETYPE,1,[Define if the GD library supports FREETYPE])
	    fi
	    if test "$f" = "GD_GIF"; then
                AC_DEFINE_UNQUOTED(HAVE_GD_GIF,1,[Define if the GD library supports GIF])
	    fi
	    if test "$f" = "GD_GIFANIM"; then
                AC_DEFINE_UNQUOTED(HAVE_GD_GIFANIM,1,[Define if the GD library supports GIFANIM])
	    fi
	    if test "$f" = "GD_OPENPOLYGON"; then
                AC_DEFINE_UNQUOTED(HAVE_GD_OPENPOLYGON,1,[Define if the GD library supports OPENPOLYGON])
	    fi
        done

        # GD version check >= 2.0.34  else revert to --with-mylibgd
        REQ_GD_MAJORVERSION=2
        REQ_GD_MINORVERSION=0
        REQ_GD_REVISION=34
        if test $GD_MAJORVERSION -lt $REQ_GD_MAJORVERSION; then
	    with_mylibgd=yes
        else
            if test $GD_MAJORVERSION -eq $REQ_GD_MAJORVERSION; then
                if test $GD_MINORVERSION -lt $REQ_GD_MINORVERSION; then
	            with_mylibgd=yes
	        else
                    if test $GD_MINORVERSION -eq $REQ_GD_MINORVERSION; then
		        if test $GD_REVISION -lt $REQ_GD_REVISION; then
		            with_mylibgd=yes
		        fi
	            fi
	        fi
            fi
        fi
        if test "x$with_mylibgd" = "xyes"; then
            AC_MSG_WARN(GD library version < $REQ_GD_MAJORVERSION.$REQ_GD_MINORVERSION.$REQ_GD_REVISION)
        fi
    else
        with_mylibgd=yes
        AC_MSG_WARN(No gdlib-config found.)
    fi
fi
fi

if test "x$with_mylibgd" = "xno"; then
if test "x$with_libgd" = "xyes"; then
# see if we can use the external gd lib
    save_CPPFLAGS=$CPPFLAGS
    save_LDFLAGS=$LDFLAGS
    AC_ARG_WITH(gdincludedir,
      [AC_HELP_STRING([--with-gdincludedir=DIR], [use GD includes from DIR])],
      GD_INCLUDES="-I$withval")
    CPPFLAGS="$CPPFLAGS $GD_INCLUDES"
    AC_ARG_WITH(gdlibdir,
      [AC_HELP_STRING([--with-gdlibdir=DIR], [use GD libraries from DIR])],
      [GD_LDFLAGS="-L$withval"])
    LDFLAGS="$LDFLAGS $GD_LDFLAGS"
    AC_CHECK_HEADER(gd.h,
    AC_CHECK_LIB(gd,main,
        [GD_LIBS="$GD_LDFLAGS -lgd $GD_LIBS"
            AC_DEFINE_UNQUOTED(HAVE_LIBGD,1,[Define if you have the GD library])],
        AC_MSG_WARN(Optional GD library not available), $GD_LIBS),
	    AC_MSG_WARN(Optional GD library not available - no gd.h))
    LDFLAGS=$save_LDFLAGS
    CPPFLAGS=$save_CPPFLAGS
fi
else
    # using the internal libgd
    AC_MSG_WARN(Using internal libgd source.)
    AC_DEFINE_UNQUOTED(HAVE_GD_GIF,1,[Define if the GD library has the GD_GIF feature])
    AC_DEFINE_UNQUOTED(HAVE_LIBGD,1,[Define if either internal or external GD library is availabel])
    GD_INCLUDES='-I$(top_srcdir)/lib/gd'
    GD_LIBS='$(top_builddir)/lib/gd/libgvgd.la'
fi
AC_SUBST(GD_INCLUDES)
AC_SUBST(GD_LIBS)
AM_CONDITIONAL(WITH_LIBGD, [test "x$with_libgd" = "xyes"])
AM_CONDITIONAL(WITH_MYLIBGD, [test "x$with_mylibgd" = "xyes"])

dnl ----------------------------------
dnl INCLUDES and LIBS for FREETYPE2 (only with mylibgd)

if test "x$with_mylibgd" = "xyes"; then
if test "x$HAVE_FREETYPE2" != "x"; then
AC_DEFINE_UNQUOTED(HAVE_GD_FREETYPE,1,[Define if the GD library has the GD_FREETYPE feature])
fi
fi

dnl ----------------------------------
dnl INCLUDES and LIBS for FONTCONFIG (only with mylibgd)

if test "x$with_mylibgd" = "xyes"; then
if test "x$HAVE_FONTCONFIG" != "x"; then
AC_DEFINE_UNQUOTED(HAVE_GD_FONTCONFIG,1,[Define if the GD library has the GD_FONTCONFIG feature])
fi
fi

dnl -----------------------------------
dnl INCLUDES and LIBS for ICONV (only with mylibgd)

if test "x$with_mylibgd" = "xyes"; then

save_CPPFLAGS=$CPPFLAGS
save_LDFLAGS=$LDFLAGS
AC_ARG_WITH(iconvincludedir,
  [AC_HELP_STRING([--with-iconvincludedir=DIR], [use ICONV includes from DIR])],
  [ICONV_INCLUDES="-I$withval"])
CPPFLAGS="$CPPFLAGS $ICONV_INCLUDES"
AC_ARG_WITH(iconvlibdir,
  [AC_HELP_STRING([--with-iconvlibdir=DIR], [use ICONV libraries from DIR])],
  [ICONV_LIBS="-L$withval"])
LDFLAGS="$LDFLAGS $ICONV_LIBS"

# Some systems have iconv in a separate library (e.g. Tru64 UNIX)
AM_ICONV
if test -n "$LIBICONV" ; then
  ICONV_LIBS="$LIBICONV"
else
  ICONV_LIBS=""
fi

AC_CHECK_HEADERS(iconv.h,
  [AC_MSG_CHECKING(whether iconv.h defines iconv_t)
   AC_EGREP_HEADER([typedef.*iconv_t],iconv.h,
     [AC_MSG_RESULT(yes)
      AC_DEFINE(HAVE_ICONV_T_DEF, 1,
                [Define if <iconv.h> defines iconv_t.])],
      AC_MSG_RESULT(no))])

LDFLAGS=$save_LDFLAGS
CPPFLAGS=$save_CPPFLAGS
AC_SUBST(ICONV_INCLUDES)
AC_SUBST(ICONV_LIBS)

fi

dnl -----------------------------------
dnl INCLUDES and LIBS for PNG (only with mylibgd)

if test "x$with_mylibgd" = "xyes"; then

save_CPPFLAGS=$CPPFLAGS
save_LDFLAGS=$LDFLAGS
AC_PATH_PROG(LIBPNG_CONFIG,libpng-config)
if test -n "$LIBPNG_CONFIG"; then
        PNG_INCLUDES=`$LIBPNG_CONFIG --cflags`
        PNG_PREFIX=`$LIBPNG_CONFIG --prefix`
	if test "${PNG_PREFIX}/lib" != "/usr/lib" ; then
  		PNG_LIBS="-L${PNG_PREFIX}/lib"
 	fi
else
	PNG_INCLUDES=""
fi
AC_ARG_WITH(pngincludedir,
  [AC_HELP_STRING([--with-pngincludedir=DIR], [use PNG includes from DIR])],
  [PNG_INCLUDES="-I$withval"])
CPPFLAGS="$CPPFLAGS $PNG_INCLUDES $Z_INCLUDES"
AC_ARG_WITH(pnglibdir,
  [AC_HELP_STRING([--with-pnglibdir=DIR], [use PNG libraries from DIR])],
  [PNG_LIBS="-L$withval"])
LDFLAGS="$LDFLAGS $PNG_LIBS $Z_LIBS"
AC_CHECK_HEADER(png.h,
	AC_CHECK_LIB(png,main,
		[PNG_LIBS="$PNG_LIBS -lpng"
		AC_DEFINE_UNQUOTED(HAVE_GD_PNG,1,[Define if the GD library has the GD_PNG feature])
		AC_DEFINE_UNQUOTED(HAVE_LIBPNG,1,[Define if you have the PNG library])],
		AC_MSG_WARN(Optional png library not available),
		[$Z_LIBS $MATH_LIBS]),
	AC_MSG_WARN(Optional png library not available - no png.h))
LDFLAGS=$save_LDFLAGS
CPPFLAGS=$save_CPPFLAGS
AC_SUBST(PNG_INCLUDES)
AC_SUBST(PNG_LIBS)

fi

dnl -----------------------------------
dnl INCLUDES and LIBS for JPEG (only with mylibgd)

if test "x$with_mylibgd" = "xyes"; then

save_CPPFLAGS=$CPPFLAGS
save_LDFLAGS=$LDFLAGS
AC_ARG_WITH(jpegincludedir,
  [AC_HELP_STRING([--with-jpegincludedir=DIR], [use JPEG includes from DIR])],
  [JPEG_INCLUDES="-I$withval"])
CPPFLAGS="$CPPFLAGS $JPEG_INCLUDES"
JPEG_LIBDIR="/usr/lib$LIBPOSTFIX"
AC_ARG_WITH(jpeglibdir,
  [AC_HELP_STRING([--with-jpeglibdir=DIR], [use JPEG libraries from DIR])],
  [JPEG_LIBDIR="$withval"])
if test "$JPEG_LIBDIR" != "/usr/lib$LIBPOSTFIX"; then
	JPEG_LIBS="-L$JPEG_LIBDIR"
	LDFLAGS="$LDFLAGS $JPEG_LIBS"
fi
if test -f "$JPEG_LIBDIR/libjpeg.la"; then
        JPEG_LIBS="$JPEG_LIBDIR/libjpeg.la"
fi
AC_CHECK_HEADER(jpeglib.h,
	AC_CHECK_LIB(jpeg,main,
		[JPEG_LIBS="$JPEG_LIBS -ljpeg"
		AC_DEFINE_UNQUOTED(HAVE_GD_JPEG,1,[Define if the GD library has the GD_JPEG feature])
		AC_DEFINE_UNQUOTED(HAVE_LIBJPEG,1,[Define if you have the JPEG library])],
			AC_MSG_WARN(Optional jpeg library not available)),
	AC_MSG_WARN(Optional jpeg library not available - no jpeglib.h))
LDFLAGS=$save_LDFLAGS
CPPFLAGS=$save_CPPFLAGS
AC_SUBST(JPEG_INCLUDES)
AC_SUBST(JPEG_LIBS)

fi

# -----------------------------------
# tclInt.h
if test -f "$TCL_SRC_DIR/generic/tclInt.h"; then
TCLINT_INCLUDES="-I$TCL_SRC_DIR/generic"
else
if test -f "/usr/include/tclInt.h"; then
TCLINT_INCLUDES=""
else
if test -f "/usr/local/include/tclInt.h"; then
TCLINT_INCLUDES="-I/usr/local/include"
else
TCLINT_INCLUDES='-I$(top_srcdir)/tclpkg/tclstubs'
fi
fi
fi
AC_SUBST(TCLINT_INCLUDES)

# -----------------------------------
# tkInt.h
if test -f "$TK_SRC_DIR/generic/tkInt.h"; then
TKINT_INCLUDES="-I$TK_SRC_DIR/generic -I$TK_SRC_DIR/unix"
else
if test -f "/usr/include/tkInt.h"; then
TKINT_INCLUDES=""
else
if test -f "/usr/local/include/tkInt.h"; then
TKINT_INCLUDES="-I/usr/local/include"
else
TKINT_INCLUDES='-I$(top_srcdir)/tclpkg/tkstubs'
fi
fi
fi
AC_SUBST(TKINT_INCLUDES)

# ----------------------------------
# tcl/tk pkgIndex.tcl generation

TCL_PKGINDEX=""
TCL_PKGINDEX_GD=""
TCL_PKGINDEX_SWIG=""
TK_PKGINDEX=""
if test "$HAVE_TCL" = "1"; then
    TCL_PKGINDEX="tcldot/pkgIndex.tcl tclpathplan/pkgIndex.tcl"
    if test "x$with_libgd" = "xyes"; then
	TCL_PKGINDEX_GD="gdtclft/pkgIndex.tcl"
    fi
    if test "x$SWIG" != "x"; then
	TCL_PKGINDEX_SWIG="gv/pkgIndex.tcl"
    fi
    if test "$HAVE_TK" = "1"; then
        TK_PKGINDEX="tkspline/pkgIndex.tcl"
    fi
fi
AC_SUBST(TCL_PKGINDEX)
AC_SUBST(TCL_PKGINDEX_GD)
AC_SUBST(TCL_PKGINDEX_SWIG)
AC_SUBST(TK_PKGINDEX)

# -----------------------------------
# Special checks

AC_MSG_CHECKING(if have working sincos())
AC_TRY_RUN([
	#include <stdio.h>
	#include <assert.h>
	#define PI 3.14159265358979323846
	extern void sincos(double x, double *s, double *c);
	int main () {
		double sinx=0.0, cosx=1.0;
		sincos(PI/2.0,&sinx,&cosx);
		assert(sinx>0.9999999);
		assert(sinx<1.0000001);
		assert(cosx<0.0000001);
		assert(cosx>(-0.0000001));
		return 0;
	}]
	,
	AC_MSG_RESULT(yes)
	AC_DEFINE_UNQUOTED(HAVE_SINCOS,1,[Define if libm provides a *working* sincos function])
	,
	AC_MSG_RESULT(no)
	,
	AC_MSG_RESULT(no - assumed because cross-compiling)
	)

# -----------------------------------
AC_MSG_CHECKING(if FILE struct contains _cnt)
AC_TRY_COMPILE(
	#include <stdio.h>
	,
	FILE *f;
	int i;
	i = f->_cnt;
	,
	AC_MSG_RESULT(yes)
	AC_DEFINE_UNQUOTED(HAVE_FILE_CNT,1,[Define if FILE structure provides _cnt])
	,
	AC_MSG_RESULT(no)
	)

# -----------------------------------

AC_MSG_CHECKING(if FILE struct contains _r)
AC_TRY_COMPILE(
	#include <stdio.h>
	,
	FILE *f;
	int i;
	i = f->_r;
	,
	AC_MSG_RESULT(yes)
	AC_DEFINE_UNQUOTED(HAVE_FILE_R,1,[Define if FILE structure provides _r])
	,
	AC_MSG_RESULT(no)
	)

# -----------------------------------

AC_MSG_CHECKING(if FILE struct contains _next)
AC_TRY_COMPILE(
	#include <stdio.h>
	,
	FILE *f;
	int i;
	i = f->_next;
	,
	AC_MSG_RESULT(yes)
	AC_DEFINE_UNQUOTED(HAVE_FILE_NEXT,1,[Define if FILE structure provides _next])
	,
	AC_MSG_RESULT(no)
	)

# -----------------------------------

AC_MSG_CHECKING(if FILE struct contains _IO_read_end)
AC_TRY_COMPILE(
	#include <stdio.h>
	,
	FILE *f;
	int i;
	i = f->_IO_read_end;
	,
	AC_MSG_RESULT(yes)
	AC_DEFINE_UNQUOTED(HAVE_FILE_IO_READ_END,1,[Define if FILE structure provides _IO_read_end])
	,
	AC_MSG_RESULT(no)
	)

# -----------------------------------

AC_MSG_CHECKING(if errno externs are declared)
AC_TRY_LINK(
	#include <stdio.h>
	#include <errno.h>
	,
	if ( errno < sys_nerr ) return sys_errlist[[errno]];
	,
	AC_MSG_RESULT(yes)
	AC_DEFINE_UNQUOTED(HAVE_ERRNO_DECL,1,[Define if errno externs are declared])
	,
	AC_MSG_RESULT(no)
	)

# -----------------------------------

AC_MSG_CHECKING(if getopt externs are declared)
AC_TRY_LINK(
	#include <stdlib.h>
	#include <stdio.h>
	#if HAVE_UNISTD_H
	#include <unistd.h>
	#endif
	,
	if ( optopt ) exit(0); else exit(1);
	,
	AC_MSG_RESULT(yes)
	AC_DEFINE_UNQUOTED(HAVE_GETOPT_DECL,1,[Define if getopt externs are declared])
	,
	AC_MSG_RESULT(no)
	)

# -----------------------------------

AC_MSG_CHECKING(if intptr_t is declared)
AC_TRY_LINK(
	#include <stdlib.h>
	#if HAVE_STDINT_H
	#include <stdint.h>
	#endif
	#if HAVE_INTTYPES_H
	#include <inttypes.h>
	#endif
	#if HAVE_UNISTD_H
	#include <unistd.h>
	#endif
	,
	intptr_t  abc;
	,
	AC_MSG_RESULT(yes)
	AC_DEFINE_UNQUOTED(HAVE_INTPTR_T,1,[Define if intptr_t is declared])
	,
	AC_MSG_RESULT(no)
	)

# -----------------------------------

AC_MSG_CHECKING(if assert needs __eprintf)
cat > conftest.$ac_ext <<EOF
#include "confdefs.h"
#include "assert.h"
int main(int argc, char **argv) {
assert(argc);
; return 0; }
EOF
${CC-cc} -c $CFLAGS $CPPFLAGS $LDFLAGS conftest.$ac_ext 1>&5
if test "x`nm conftest.o |$GREP __eprintf`" != "x"; then
    AC_MSG_RESULT(yes)
    CFLAGS="$CFLAGS -DNDEBUG"
else
    AC_MSG_RESULT(no)
fi
rm -f conftest*

# -----------------------------------------------------------------------

# Generate Makefiles
AC_CONFIG_FILES(Makefile
	doc/Makefile
	doc/info/Makefile
	contrib/Makefile
	contrib/prune/Makefile
	contrib/diffimg/Makefile
	contrib/xdot/Makefile
	contrib/xdot/libxdot.pc
	graphs/Makefile
	graphs/directed/Makefile
	graphs/undirected/Makefile
	lib/Makefile
	lib/cdt/Makefile
	lib/cdt/libcdt.pc
	lib/graph/Makefile
	lib/graph/libgraph.pc
	lib/agraph/Makefile
	lib/agraph/libagraph.pc
	lib/cgraph/Makefile
	lib/cgraph/libcgraph.pc
	lib/agutil/Makefile
	lib/gd/Makefile
	lib/pathplan/Makefile
	lib/pathplan/libpathplan.pc
	lib/ast/Makefile
	lib/sfio/Makefile
	lib/sfio/Sfio_f/Makefile
	lib/vmalloc/Makefile
	lib/dotgen/Makefile
	lib/neatogen/Makefile
	lib/fdpgen/Makefile
	lib/circogen/Makefile
	lib/twopigen/Makefile
	lib/pack/Makefile
	lib/expr/Makefile
	lib/expr/libexpr.pc
	lib/common/Makefile
	lib/ingraphs/Makefile
	lib/vpsc/Makefile
	lib/gvc/Makefile
	lib/gvc/libgvc.pc
	lib/gvc/libgvc_builtins.pc
	lib/utilities/Makefile
	lib/gui/Makefile
	lib/xdot/Makefile
	lib/filter/Makefile
	lib/topfish/Makefile
	macosx/Info.plist
	macosx/graphviz.pmdoc/01local.xml
	macosx/graphviz.pmdoc/02graphviz.xml
	plugin/Makefile
	plugin/core/Makefile
	plugin/devil/Makefile
	plugin/gdk_pixbuf/Makefile
	plugin/rsvg/Makefile
	plugin/gd/Makefile
	plugin/pango/Makefile
	plugin/ming/Makefile
	plugin/quartz/Makefile
	plugin/dot_layout/Makefile
	plugin/neato_layout/Makefile
	plugin/xlib/Makefile
	plugin/gtk/Makefile
	cmd/Makefile
	cmd/dot/Makefile
	cmd/tools/Makefile
	cmd/gvpr/Makefile
        cmd/lefty/ws/x11/libfilereq/Makefile
        cmd/lefty/ws/x11/Makefile
        cmd/lefty/ws/mswin32/Makefile
        cmd/lefty/ws/Makefile
        cmd/lefty/os/unix/Makefile
        cmd/lefty/os/mswin32/Makefile
        cmd/lefty/os/Makefile
        cmd/lefty/dot2l/Makefile
        cmd/lefty/examples/Makefile
        cmd/lefty/Makefile
        cmd/lneato/Makefile
        cmd/dotty/Makefile
        cmd/smyrna/Makefile
	tclpkg/Makefile
	tclpkg/tclstubs/Makefile
	tclpkg/tkstubs/Makefile
	tclpkg/tclhandle/Makefile
	tclpkg/gdtclft/Makefile
	tclpkg/gdtclft/demo/Makefile
	tclpkg/tcldot/Makefile
	tclpkg/tcldot/demo/Makefile
	tclpkg/tclpathplan/Makefile
	tclpkg/tclpathplan/demo/Makefile
	tclpkg/tclpathplan/demo/pathplan_data/Makefile
	tclpkg/tkspline/Makefile
	tclpkg/tkspline/demo/Makefile
	tclpkg/gv/Makefile
	tclpkg/gv/demo/Makefile
	tclpkg/gv/META.gv
	rtest/Makefile
	share/Makefile
	share/examples/Makefile
	share/icons/Makefile
	share/gui/Makefile
	graphviz.spec
	Doxyfile
	)

AC_OUTPUT
