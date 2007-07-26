# $Id$ $Revision$

# Process this file with autoconf to produce a configure script
AC_PREREQ(2.54)

dnl ===========================================================================

# Package name and version number, (as distinct from shared library version)
AC_INIT(graphviz, 2.13)

# libtool shared library version

# Increment if the interface has additions, changes, removals.
LT_CURRENT=4

# Increment any time the source changes; set to
# 0 if you increment CURRENT
LT_REVISION=0

# Increment if any interfaces have been added; set to 0
# if any interfaces have been removed. removal has
# precedence over adding, so set to 0 if both happened.
LT_AGE=0

VERSION_INFO="$LT_CURRENT:$LT_REVISION:$LT_AGE"
AC_SUBST(VERSION_INFO)

dnl ===========================================================================

AC_CONFIG_SRCDIR(configure.ac)
AC_CONFIG_AUX_DIR(config)

dnl -----------------------------------
dnl Checks for system type

AC_CANONICAL_HOST

# This nonsense drives me crazy!!
case "${host_os}" in
  *irix* | *nonstopux* )
    GVPLUGIN_VERSION=`expr $LT_CURRENT + 1`
    ;;
  * )
    GVPLUGIN_VERSION=$LT_CURRENT
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
	PATHSEPARATOR=";"
	;;
  *cygwin* )
	CYGWIN=yes
	DEFAULT_FONTPATH="C:/WINDOWS/FONTS;C:/WINNT/Fonts;C:/winnt/fonts"
	PATHSEPARATOR=";"
	AC_DEFINE(NO_POSTSCRIPT_ALIAS,1,[Alias PostScript font names to URW type 1 fonts])
	;;
  *mingw32* )
	MINGW32=yes
	DEFAULT_FONTPATH="C:/WINDOWS/FONTS;C:/WINNT/Fonts;C:/winnt/fonts"
	PATHSEPARATOR=";"
	;;
  *darwin* )
	DEFAULT_FONTPATH="~/Library/Fonts:/Library/Fonts:/Network/Library/Fonts:/System/Library/Fonts"
	PATHSEPARATOR=":"
	AC_DEFINE_UNQUOTED(DARWIN_DYLIB,"$DARWIN_DYLIB",[Define for Darwin-style shared library names.])
	;;
  * ) # This sucks!  Whatever happened to standards for file system layout?
	DEFAULT_FONTPATH="/usr/share/fonts/default/Type1:/usr/X11R6/lib/X11/fonts/TrueType:/usr/X11R6/lib/X11/fonts/truetype:/usr/X11R6/lib/X11/fonts/TTF:/usr/share/fonts/TrueType:/usr/share/fonts/truetype:/usr/openwin/lib/X11/fonts/TrueType:/usr/X11R6/lib/X11/fonts/Type1:/usr/common/share/fonts/ttf";
	PATHSEPARATOR=":"
	;;
esac
AC_SUBST(CYGWIN)
AC_SUBST(MINGW32)
AC_SUBST(DARWIN)
AC_DEFINE_UNQUOTED(DEFAULT_FONTPATH,"$DEFAULT_FONTPATH",[Path to TrueType fonts.])
AC_DEFINE_UNQUOTED(PATHSEPARATOR,"$PATHSEPARATOR",[Path separator character.])
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

# automatically check for other dependencies in $PREFIX first
if test "x${prefix}" != "x/usr"; then
	CPPFLAGS="$CPPFLAGS -I${prefix}/include"
	LDFLAGS="$LDFLAGS -L${prefix}/lib"
	PKG_CONFIG_PATH="$PREFIX/lib/pkgconfig:$PKG_CONFIG_PATH"
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
                                                                                
AC_TYPE_MODE_T
AC_TYPE_PID_T
AC_TYPE_SIZE_T
AC_TYPE_UID_T

dnl -----------------------------------
dnl Checks for header files

# AC_STDC_HEADERS
# AC_HAVE_HEADERS(string.h)
AC_CHECK_HEADERS(stdarg.h stddef.h stddef.h stdlib.h stdint.h malloc.h \
	search.h getopt.h pthread.h values.h float.h limits.h termios.h \
	errno.h time.h unistd.h fenv.h strings.h inttypes.h \
	sys/time.h sys/times.h sys/types.h sys/select.h fpu_control.h \
	sys/fpu.h strings.h sys/socket.h sys/stat.h sys/mman.h \
	sys/ioctl.h sys/inotify.h)
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
	strtoul strtoll strtoull uname])

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

dnl -----------------------------------
dnl old codegens

AC_ARG_WITH(codegens,
  [AC_HELP_STRING([--with-codegens],
                  [use old codegens])])

### default is to build with codegens
if test "x$with_codegens" != "xno"; then
	with_codegens=yes
fi

if test "x$with_codegens" = "xyes"; then
  AC_DEFINE(WITH_CODEGENS,1,[Define if you want the old codegens])
fi
AM_CONDITIONAL(WITH_CODEGENS, [test "x$with_codegens" = "xyes"])

dnl -----------------------------------
dnl ElectricFence support for debugging

AC_ARG_WITH(efence,
	 [  --with-efence           use efence for debugging memory use],
	EFENCE=true,)

if test "x${EFENCE}" != "x"; then
	LIBS="$LIBS -lefence"
fi

dnl -----------------------------------
dnl Don't compile X programs (and emit a warning message)
dnl if X isn't available.
dnl Checks for X header files.

AC_PATH_XTRA
if test "X$no_x" = Xyes; then
  AC_MSG_WARN([X11 not available.])
  X_SUBDIRS=""
fi
AM_CONDITIONAL(WITH_X, [test "X$no_x" != "Xyes"])

dnl -----------------------------------
dnl check for SWIG - needed for script-language bindings

AC_ARG_ENABLE(swig,
  [AC_HELP_STRING([--disable-swig], [don't support any swig-generated language bindings])])
if test "x$enable_swig" != "xno"; then
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
  [AC_HELP_STRING([--disable-sharp], [don't support C# language bindings])])
if test "x$enable_sharp" != "xno"; then
if test `$SWIG -help 2>&1 | $GREP -c '\-csharp *- Generate'` = 0; then
	SHARP=
	AC_MSG_WARN([swig does not support csharp. The SHARP packages will not be built])
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
  [AC_HELP_STRING([--disable-guile], [don't support guile language bindings])])
if test "x$enable_guile" != "xno"; then
if test `$SWIG -help 2>&1 | $GREP -c '\-guile *- Generate'` = 0; then
	GUILE=
	AC_MSG_WARN([swig does not support guile. The GUILE packages will not be built])
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
dnl INCLUDES and LIBS for IO

if test "x$SWIG" != "x"; then
AC_ARG_ENABLE(io,
  [AC_HELP_STRING([--disable-io], [don't support io language bindings])])
if test "x$enable_io" != "xno"; then
if test `$SWIG -help 2>&1 | $GREP -c '\-io *- Generate'` = 0; then
	IO=
	AC_MSG_WARN([swig does not support io. The IO packages will not be built])
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
  [AC_HELP_STRING([--disable-java], [don't support java language bindings])])
if test "x$enable_java" != "xno"; then
if test `$SWIG -help 2>&1 | $GREP -c '\-java *- Generate'` = 0; then
	JAVA=
	AC_MSG_WARN([swig does not support java. The JAVA packages will not be built])
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
  [AC_HELP_STRING([--disable-lua], [don't support lua language bindings])])
if test "x$enable_lua" != "xno"; then
if test `$SWIG -help 2>&1 | $GREP -c '\-lua *- Generate'` = 0; then
	LUA=
	AC_MSG_WARN([swig does not support lua. The LUA packages will not be built])
	enable_lua=no
#else
#AC_CHECK_PROG(LUA,lua,lua)
#LUA_INCLUDES=
#LUA_LIBS=
#AC_CHECK_HEADER(lua.h,,[
#  AC_MSG_WARN([Unable to find header lua.h. The LUA packages will not be built])
#  LUA=
#  ])
#AC_SUBST(LUA_INCLUDES)
#AC_SUBST(LUA_LIBS)
fi
fi
fi

#Lua is enabled by default
if test "x$enable_lua" != "xno" ; then
    # Not needed for Graphviz: enable_plugins="yes"

    ac_save_CPPFLAGS="$CPPFLAGS"
    ac_save_CFLAGS="$CFLAGS"
    ac_save_LDFLAGS="$LDFLAGS"

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
	    AC_MSG_CHECKING(for Lua headers and librairies with pkg-config)
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
            AC_MSG_CHECKING(for Lua headers and librairies with lua-config)
	    echo
            LUA_INCLUDES=`$LUACONFIG --include`
            LUA_LIBS=`$LUACONFIG --libs`
	    LUA_VERSION="5.0.x"
        fi
    fi

    if test "x$LUA_INCLUDES" = "x" -o "x$LUA_LIBS" = "x" ; then
        AC_MSG_CHECKING(for Lua headers and librairies)
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
*** Lua (>=5.0) headers and/or librairies couldn't be found in your system.
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
  [AC_HELP_STRING([--disable-ocaml], [don't support ocaml language bindings])])
if test "x$enable_ocaml" != "xno"; then
if test `$SWIG -help 2>&1 | $GREP -c '\-ocaml *- Generate'` = 0; then
	OCAML=
	AC_MSG_WARN([swig does not support ocaml. The OCAML packages will not be built])
else
AC_CHECK_PROG(OCAML,ocaml,ocaml)
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

dnl -----------------------------------
dnl INCLUDES and LIBS for PERL

if test "x$SWIG" != "x"; then
AC_ARG_ENABLE(perl,
  [AC_HELP_STRING([--disable-perl], [don't support perl language bindings])])
if test "x$enable_perl" != "xno"; then
AC_CHECK_PROG(PERL,perl,perl)
if test -n "$PERL" && $PERL -e 'exit !($] <= 5.004)' > /dev/null 2>&1 ; then
	PERL=
	AC_MSG_WARN([perl is too old. The PERL packages will not be built])
fi
if test `$SWIG -help 2>&1 | $GREP -c '\-perl *- Generate'` = 0; then
	PERL=
	AC_MSG_WARN([swig does not support perl. The PERL packages will not be built])
fi
if test "x$PERL" != "x"; then
  PERL_ARCHLIB=`$PERL -e 'use Config; print $Config{archlib};'`
  PERL_INCLUDES=-I$PERL_ARCHLIB/CORE
  PERL_LIBS="-L$PERL_ARCHLIB/CORE -lperl"
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
fi
fi
AM_CONDITIONAL(WITH_PERL, [test "x$PERL" != "x"])

dnl -----------------------------------
dnl INCLUDES and LIBS for PHP

if test "x$SWIG" != "x"; then
AC_ARG_ENABLE(php,
  [AC_HELP_STRING([--disable-php], [don't support php language bindings])])
if test "x$enable_php" != "xno"; then
if test `$SWIG -help 2>&1 | $GREP -c '\-php[[5 ]]*- Generate'` = 0; then
	PHP=
	AC_MSG_WARN([swig does not support php. The PHP packages will not be built])
else
AC_CHECK_PROG(PHP,php,php)
if test -d /usr/include/php5; then
PHP_INCLUDES="-I/usr/include/php5 -I/usr/include/php5/main -I/usr/include/php5/Zend -I/usr/include/php5/TSRM"
else
PHP_INCLUDES="-I/usr/include/php -I/usr/include/php/main -I/usr/include/php/Zend -I/usr/include/php/TSRM"
fi
PHP_LIBS=
save_CPPFLAGS=$CPPFLAGS
CPPFLAGS="$CPPFLAGS $PHP_INCLUDES"
AC_CHECK_HEADER(zend.h,,[
  AC_MSG_WARN([Unable to find header zend.h. The PHP packages will not be built])
  PHP=
  ])
CPPFLAGS=$save_CPPFLAGS
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
  [AC_HELP_STRING([--disable-python], [don't support python language bindings])])
if test "x$enable_python" != "xno"; then
if test `$SWIG -help 2>&1 | $GREP -c '\-python *- Generate'` = 0; then
	PYTHON=
	AC_MSG_WARN([swig does not support python. The PYTHON packages will not be built])
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
    PYTHON_LIBS="-L$PYTHON_PREFIX/lib$LIBPOSTFIX -lpython$PYTHON_VERSION_SHORT"
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
fi
fi
fi
AM_CONDITIONAL(WITH_PYTHON, [test "x$PYTHON" != "x"])

dnl -----------------------------------
dnl INCLUDES and LIBS for RUBY

if test "x$SWIG" != "x"; then
AC_ARG_ENABLE(ruby,
  [AC_HELP_STRING([--disable-ruby], [don't support ruby language bindings])])
if test "x$enable_ruby" != "xno"; then
if test `$SWIG -help 2>&1 | $GREP -c '\-ruby *- Generate'` = 0; then
	RUBY=
	AC_MSG_WARN([swig does not support ruby. The RUBY packages will not be built])
else
AC_CHECK_PROG(RUBY,ruby,ruby)
if test "x$RUBY" != "x"; then
  RUBY_INCLUDES="-I`$RUBY $srcdir/config/config_ruby.rb INCLUDES`"
  RUBY_LIBS="-L`$RUBY $srcdir/config/config_ruby.rb lib` `$RUBY -rrbconfig -e \"puts Config::CONFIG[['LIBRUBYARG_SHARED']]\"`"
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
fi
fi
fi
AM_CONDITIONAL(WITH_RUBY, [test "x$RUBY" != "x"])

dnl -----------------------------------
dnl INCLUDES and LIBS for TCL

AC_ARG_ENABLE(tcl,
  [AC_HELP_STRING([--disable-tcl], [don't support tcl language bindings])])
if test "x$enable_tcl" != "xno"; then
# start by assuming we have it
HAVE_TCL=1 
else
HAVE_TCL=0
fi

AC_ARG_WITH(tclsh,
    [  --with-tclsh=PROG       build graphviz for specific tclsh],
    TCLSH=$withval,)

if test "x$TCLSH" = "x"; then
    AC_PATH_PROG(TCLSH,tclsh8.4)
    if test "x$TCLSH" = "x"; then
        AC_PATH_PROG(TCLSH,tclsh8.3)
        if test "x$TCLSH" = "x"; then
            AC_PATH_PROG(TCLSH,tclsh)
        fi
    fi
    if test "x$TCLSH" = "x"; then
        AC_MSG_WARN([Unable to find a tclsh. The Tcl packages will not be built])
        HAVE_TCL=0
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
fi

if test "$HAVE_TCL" = "1"; then
    # TCL STUBS support is required
    if test "x${TCL_SUPPORTS_STUBS}" != "x1"; then
        AC_MSG_WARN([Tcl does not have STUBs support, perhaps it is too old?  The Tcl packages will not be built])
        HAVE_TCL=0
    fi
fi

AC_SUBST(HAVE_TCL)

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
        [  --with-wish=PROG        build graphviz for specific wish],
        WISH=$withval,)
    if test "x$WISH" = "x"; then
        AC_PATH_PROG(WISH,wish8.4)
        if test "x$WISH" = "x"; then
    	    AC_PATH_PROG(WISH,wish8.3)
    	    if test "x$WISH" = "x"; then
                AC_PATH_PROG(WISH,wish)
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
	TK_INCLUDES="-I${WISH_EXEC_PREFIX}/include/tk${TK_VERSION_FOUND} -I${WISH_EXEC_PREFIX}/include/tk${TK_VERSION_FOUND}/generic"
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
  [  --with-extraincludedir=DIR use extra includes from DIR],
  [EXTRAINCLUDEDIR=$withval
   CPPFLAGS="$CPPFLAGS -I$withval"],)

AC_ARG_WITH(extralibdir,
  [  --with-extralibdir=DIR  use extra libraries from DIR],
  LDFLAGS="$LDFLAGS -L$withval",)

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
	[  --with-expatincludedir=DIR use EXPAT includes from DIR],
	[EXPAT_INCLUDES="-I$withval"])
CPPFLAGS="$CPPFLAGS $EXPAT_INCLUDES"
EXPAT_LIBDIR="/usr/lib$LIBPOSTFIX"
AC_ARG_WITH(expatlibdir,
	[  --with-expatlibdir=DIR  use EXPAT libraries from DIR],
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
dnl INCLUDES and LIBS for XPM
#
# we don't want an X11 dependency in libgd
# so we set HAVE_LIBXPMFORLEFTY instead of the usual HAVE_LIBXPM
#

save_CPPFLAGS=$CPPFLAGS
save_LDFLAGS=$LDFLAGS
AC_ARG_WITH(Xpmincludedir,
	[  --with-Xpmincludedir=DIR use Xpm includes from DIR],
	[XPM_INCLUDES="-I$withval"])
AC_ARG_WITH(Xpmlibdir,
	[  --with-Xpmlibdir=DIR    use Xpm libraries from DIR],
	[XPM_LIBS="-L$withval"])
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

save_CPPFLAGS=$CPPFLAGS
save_LDFLAGS=$LDFLAGS
AC_ARG_WITH(Xawincludedir,
	[  --with-Xawincludedir=DIR use XAW includes from DIR],
	[XAW_INCLUDES="-I$withval"])
CPPFLAGS="$CPPFLAGS $XAW_INCLUDES $X_CFLAGS"
AC_ARG_WITH(Xawlibdir,
	[  --with-Xawlibdir=DIR    use XAW libraries from DIR],
	[XAW_LIBS="-L$withval"])
LDFLAGS="$LDFLAGS $XAW_LIBS $XPM_LIBS $X_LIBS $X_EXTRA_LIBS $X_PRE_LIBS $SOCKET_LIBS"
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
	     [XAW_LIBS="$XAW_LIBS -lXaw"],
	     [with_xaw=no
	      AC_MSG_WARN(Optional Xaw library not available)],
	      [-lXmu -lXt -lX11 -lXext $SOCKET_LIBS])
fi
LDFLAGS=$save_LDFLAGS
CPPFLAGS=$save_CPPFLAGS
AC_SUBST(XAW_INCLUDES)
AC_SUBST(XAW_LIBS)
AM_CONDITIONAL(WITH_XAW, [test "x$with_xaw" != "xno"])
if test "x$with_xaw" = "xno" && "X$no_x" != Xyes; then
	AC_MSG_WARN(Lefty cannot be built)
fi

dnl -----------------------------------
dnl INCLUDES and LIBS for Z

save_CPPFLAGS=$CPPFLAGS
save_LDFLAGS=$LDFLAGS
AC_ARG_WITH(zincludedir,
	[  --with-zincludedir=DIR  use Z includes from DIR],
	[Z_INCLUDES="-I$withval"])
CPPFLAGS="$CPPFLAGS $Z_INCLUDES"
AC_ARG_WITH(zlibdir,
	[  --with-zlibdir=DIR      use Z libraries from DIR],
	[Z_LIBS="-L$withval"])
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
dnl DIGCOLA (enabled by default)
AC_ARG_WITH(digcola,
  [AC_HELP_STRING([--with-digcola],
                  [with DIGCOLA features in neato layout engine])])
if test "x$with_digcola" != "xno"; then
	AC_DEFINE_UNQUOTED(DIGCOLA,1,[Define if you want DIGCOLA])
else
	AC_MSG_WARN(Optional DIGCOLA is disabled)
fi

dnl -----------------------------------
dnl IPSEPCOLA (disable by default)
AC_ARG_WITH(ipsepcola,
  [AC_HELP_STRING([--with-ipsepcola],
                  [with IPSEPCOLA features in neato layout engine])])
if test "x$with_digcola" = "xno"; then
	with_ipsepcola=no

fi
if test "x$with_ipsepcola" != "xyes"; then
	IPSEPCOLA_INCLUDES=
	IPSEPCOLA_LIBS=
else
	AC_MSG_WARN(Optional IPSEPCOLA is enabled)
	AC_DEFINE_UNQUOTED(IPSEPCOLA,1,[Define if you want IPSEPCOLA])
	IPSEPCOLA_INCLUDES='-I$(top_srcdir)/lib/vpsc'
	IPSEPCOLA_LIBS='$(top_builddir)/lib/vpsc/libvpsc_C.la -lstdc++'
fi
AC_SUBST(IPSEPCOLA_INCLUDES)
AC_SUBST(IPSEPCOLA_LIBS)
                                                                                
AM_CONDITIONAL(WITH_IPSEPCOLA, [test "x$with_ipsepcola" = "xyes"])

dnl -----------------------------------
dnl INCLUDES and LIBS for PANGO, CAIRO et.al.
                                                                                
AC_ARG_WITH(pangocairo,
  [AC_HELP_STRING([--with-pangocairo],
                  [use pangocairo library])])

### default is to build with pangocairo
if test "x$with_pangocairo" != "xno"; then
	with_pangocairo=yes
fi

if test "x$with_pangocairo" = "xyes"; then
PKG_CHECK_MODULES(PANGOCAIRO, [pangocairo >= 1.0],
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
  [AC_HELP_STRING([--with-freetype2],
                  [use freetype2 library])])

### default is to build with freetype2
if test "x$with_freetype2" != "xno"; then
	with_freetype2=yes
fi

if test "x$with_freetype2" = "xyes"; then
PKG_CHECK_MODULES(FREETYPE2, [freetype2],
	[HAVE_FREETYPE2=1
	AC_DEFINE_UNQUOTED(HAVE_FREETYPE2,1,
		[Define if you have the freetype2 library])],
	[AC_MSG_WARN(freetype2 library not available)])
AC_SUBST(FREETYPE2_CFLAGS)
AC_SUBST(FREETYPE2_LIBS)
fi
AM_CONDITIONAL(WITH_FREETYPE2, [test "x$HAVE_FREETYPE2" != "x"])

dnl -----------------------------------
dnl INCLUDES and LIBS for FONTCONFIG
                                                                                
AC_ARG_WITH(fontconfig,
  [AC_HELP_STRING([--with-fontconfig],
                  [use fontconfig library])])

### default is to build with fontconfig
if test "x$with_fontconfig" != "xno"; then
	with_fontconfig=yes
fi

if test "x$with_fontconfig" = "xyes"; then
PKG_CHECK_MODULES(FONTCONFIG, [fontconfig],
	[HAVE_FONTCONFIG=1
	AC_DEFINE_UNQUOTED(HAVE_FONTCONFIG,1,
		[Define if you have the fontconfig library])],
	[AC_MSG_WARN(fontconfig library not available)])
AC_SUBST(FONTCONFIG_CFLAGS)
AC_SUBST(FONTCONFIG_LIBS)
fi
AM_CONDITIONAL(WITH_FONTCONFIG, [test "x$HAVE_FONTCONFIG" != "x"])

dnl -----------------------------------
dnl INCLUDES and LIBS for GTK.

AC_ARG_WITH(gtk,
  [AC_HELP_STRING([--with-gtk],
                  [use gtk+ library])])

### default is to build with gtk+
if test "x$with_gtk" != "xno"; then
	with_gtk=yes
fi

if test "x$with_gtk" = "xyes"; then
PKG_CHECK_MODULES(GTK, [gtk+-2.0 >= 2.7.0],
        [HAVE_GTK=1
        AC_DEFINE_UNQUOTED(HAVE_GTK,1,
                [Define if you have the gtk+ library])],
        [AC_MSG_WARN(gtk+ library not available. desktop features will not be built)])
AC_SUBST(GTK_CFLAGS)
AC_SUBST(GTK_LIBS)
fi
AM_CONDITIONAL(WITH_GTK, [test "x$HAVE_GTK" != "x"])

dnl -----------------------------------
dnl INCLUDES and LIBS for GNOMEUI.

AC_ARG_WITH(gnomeui,
  [AC_HELP_STRING([--with-gnomeui],
                  [use gnomeui library])])

### default is to build with gnomeui
if test "x$with_gnomeui" != "xno"; then
	with_gnomeui=yes
fi

if test "x$with_gnomeui" = "xyes"; then

PKG_CHECK_MODULES(GNOMEUI, [libgnomeui-2.0],
        [HAVE_GNOMEUI=1
        AC_DEFINE_UNQUOTED(HAVE_GNOMEUI,1,
                [Define if you have the gnomeui library])],
        [AC_MSG_WARN(gnomeui library not available. using alternate code for firefox interaction)])
AC_SUBST(GNOMEUI_CFLAGS)
AC_SUBST(GNOMEUI_LIBS)
fi
AM_CONDITIONAL(WITH_GNOMEUI, [test "x$HAVE_GNOMEUI" != "x"])

dnl -----------------------------------
dnl INCLUDES and LIBS for GLADE.

GETTEXT_PACKAGE=glade-2.0
AC_SUBST(GETTEXT_PACKAGE)
AC_DEFINE_UNQUOTED(GETTEXT_PACKAGE,"$GETTEXT_PACKAGE", [Gettext package.])

ALL_LINGUAS=""
AM_GLIB_GNU_GETTEXT

dnl -----------------------------------
dnl INCLUDES and LIBS for LIBMING (SWF renderer)
                                                                                
GV_PATH_MING
AM_CONDITIONAL(WITH_MING, [ test x"$MAKESWF" != x ])

#AC_ARG_WITH(libming,
#  [AC_HELP_STRING([--with-libming],
#                  [use libming library])])
#
#### default is to build with libming
#if test "x$with_libming" != "xno"; then
#	with_libming=yes
#fi
#
#if test "x$with_libming" = "xyes"; then
#PKG_CHECK_MODULES(LIBMING, [libming >= 0.4],
#	[HAVE_LIBMING=1
#	AC_DEFINE_UNQUOTED(HAVE_LIBMING,1,
#		[Define if you have the libming library for SWF support])],
#	[AC_MSG_WARN(libming library not available)])
#AC_SUBST(LIBMING_CFLAGS)
#AC_SUBST(LIBMING_LIBS)
#fi
#AM_CONDITIONAL(WITH_LIBMING, [test "x$HAVE_LIBMING" != "x"])

dnl -----------------------------------
dnl INCLUDES and LIBS for GD
                                                                                
AC_ARG_WITH(libgd,
  [AC_HELP_STRING([--with-libgd],
                  [use gd library])])

AC_ARG_WITH(mylibgd,
  [AC_HELP_STRING([--with-mylibgd],
                  [use internal gd library in preference to any installed libgd])])

### default is to build with libgd
if test "x$with_libgd" != "xno"; then
	with_libgd=yes
fi
if test "x$with_mylibgd" = "xyes"; then
	with_libgd=yes
fi

if test "x$with_libgd" = "xyes"; then
if test "x$with_mylibgd" != "xyes"; then
    AC_PATH_PROG(GDLIB_CONFIG,gdlib-config)
    if test -n "$GDLIB_CONFIG"; then
        GD_INCLUDES=`$GDLIB_CONFIG --includes`
        GD_LDFLAGS=`$GDLIB_CONFIG --ldflags`
        GD_LIBS=`$GDLIB_CONFIG --libs`
        GD_MAJORVERSION=`$GDLIB_CONFIG --majorversion`
        GD_MINORVERSION=`$GDLIB_CONFIG --minorversion`
        GD_REVISION=`$GDLIB_CONFIG --revision`
        for f in `$GDLIB_CONFIG --features` ; do
                AC_DEFINE_UNQUOTED(HAVE_$f,1,[Define if the GD library has the $f feature])
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

if test "x$with_mylibgd" != "xyes"; then
if test "x$with_libgd" = "xyes"; then
# see if we can use the external gd lib
    save_CPPFLAGS=$CPPFLAGS
    save_LDFLAGS=$LDFLAGS
    AC_ARG_WITH(gdincludedir,
        [  --with-gdincludedir=DIR use GD includes from DIR],
        GD_INCLUDES="-I$withval")
    CPPFLAGS="$CPPFLAGS $GD_INCLUDES"
    AC_ARG_WITH(gdlibdir,
        [  --with-gdlibdir=DIR     use GD libraries from DIR],
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
AC_SUBST(HAVE_LIBGD)
                                                                                
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
     [  --with-iconvincludedir=DIR use ICONV includes from DIR],
     [ICONV_INCLUDES="-I$withval"])
CPPFLAGS="$CPPFLAGS $ICONV_INCLUDES"
AC_ARG_WITH(iconvlibdir,
     [  --with-iconvlibdir=DIR  use ICONV libraries from DIR],
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
	[  --with-pngincludedir=DIR use PNG includes from DIR],
	PNG_INCLUDES="-I$withval")
CPPFLAGS="$CPPFLAGS $PNG_INCLUDES $Z_INCLUDES"
AC_ARG_WITH(pnglibdir,
	[  --with-pnglibdir=DIR    use PNG libraries from DIR],
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
	[  --with-jpegincludedir=DIR use JPEG includes from DIR],
	[JPEG_INCLUDES="-I$withval"])
CPPFLAGS="$CPPFLAGS $JPEG_INCLUDES"
JPEG_LIBDIR="/usr/lib$LIBPOSTFIX"
AC_ARG_WITH(jpeglibdir,
	[  --with-jpeglibdir=DIR   use JPEG libraries from DIR],
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
TKINT_INCLUDES="-I$TK_SRC_DIR/generic"
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
	graphs/Makefile
	graphs/directed/Makefile
	graphs/undirected/Makefile
	lib/Makefile
	lib/cdt/Makefile
	lib/cdt/libcdt.pc
	lib/agraph/Makefile
	lib/agraph/libagraph.pc
	lib/graph/Makefile
	lib/graph/libgraph.pc
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
	plugin/Makefile
	plugin/core/Makefile
	plugin/gd/Makefile
	plugin/pango/Makefile
	plugin/ming/Makefile
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
	graphviz.spec
	Doxyfile
	)

AC_OUTPUT
