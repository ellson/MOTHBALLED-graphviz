# Process this file with autoconf to produce a configure script
AC_INIT(graphviz, 2.3)
AC_CONFIG_SRCDIR(configure.ac)
AC_CONFIG_AUX_DIR(config)

dnl -----------------------------------
dnl libtool ltdl on-demand plugin loading

AC_ARG_ENABLE(ltdl,
  [AC_HELP_STRING([--disable-ltdl], [don't support on-demand plugin loading])])
if test "x$enable_ltdl" = "xno"; then
  AC_DEFINE(DISABLE_LTDL,1,[Define if you don't want on-demand plugin loading])
else
  AC_CONFIG_SUBDIRS(libltdl)
  AC_LIBTOOL_DLOPEN
  AC_LIBLTDL_CONVENIENCE
fi
AM_CONDITIONAL(DISABLE_LTDL, [test "x$enable_ltdl" = "xno"])
AC_SUBST(INCLTDL)
AC_SUBST(LIBLTDL)

# Checks for system type
AC_CANONICAL_TARGET

LIBPOSTFIX=
case "${target_os}" in
  *linux* )
    case "${target_cpu}" in
      powerpc64 | s390x | x86_64 )
        LIBPOSTFIX="64"
        ;;
    esac
    ;;
esac

CYGWIN=no
MINGW32=no
DARWIN=no
case "${target_os}" in
  *cygwin* )
	CYGWIN=yes
	DEFAULT_FONTPATH="C:/WINDOWS/FONTS;C:/WINNT/Fonts;C:/winnt/fonts"
	PATHSEPARATOR=";"
	;;
  *mingw32* )
	MINGW32=yes
	DEFAULT_FONTPATH="C:/WINDOWS/FONTS;C:/WINNT/Fonts;C:/winnt/fonts"
	PATHSEPARATOR=";"
	;;
  *darwin* )
	DEFAULT_FONTPATH="~/Library/Fonts:/Library/Fonts:/Network/Library/Fonts:/System/Library/Fonts"
	PATHSEPARATOR=":"
	;;
  * ) # This sucks!  Whatever happened to standards for file system layout?
	DEFAULT_FONTPATH="/usr/X11R6/lib/X11/fonts/TrueType:/usr/X11R6/lib/X11/fonts/truetype:/usr/X11R6/lib/X11/fonts/TTF:/usr/share/fonts/TrueType:/usr/share/fonts/truetype:/usr/openwin/lib/X11/fonts/TrueType:/usr/X11R6/lib/X11/fonts/Type1:/usr/common/share/fonts/ttf";
	PATHSEPARATOR=":"
	;;
esac
AC_SUBST(CYGWIN)
AC_SUBST(MINGW32)
AC_SUBST(DARWIN)
AC_DEFINE_UNQUOTED(DEFAULT_FONTPATH,"$DEFAULT_FONTPATH",Path to TrueType fonts.)
AC_DEFINE_UNQUOTED(PATHSEPARATOR,"$PATHSEPARATOR",Path separator character.)

DEFAULT_DPI=96
AC_DEFINE_UNQUOTED(DEFAULT_DPI,$DEFAULT_DPI,Default DPI.)

AM_INIT_AUTOMAKE(AC_PACKAGE_NAME, AC_PACKAGE_VERSION)
AM_CONFIG_HEADER(config.h)

AC_DEFINE_UNQUOTED(BUILDDATE,"`date -u`",[Date (usually date of extraction from CVS).])
BUILDDATE="`date -u`"
AC_SUBST(BUILDDATE)

AC_PREFIX_DEFAULT([/usr/local])
if test "x${prefix}" = "xNONE"; then
	prefix=${ac_default_prefix}
	AC_SUBST(prefix)
fi
if test "x${prefix}" != "x/usr"; then
	CPPFLAGS="$CPPFLAGS -I${prefix}/include"
	LDFLAGS="$LDFLAGS -L${prefix}/lib"
fi

# Set of all available languages
# ALL_LINGUAS=""

# Build shared libs only
AC_DISABLE_STATIC

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
	        * )
	            CFLAGS="${CFLAGS} -Wno-unused-parameter -Wno-unknown-pragmas -Wstrict-prototypes -Wpointer-arith -Wall -ffast-math"
	            ;;
            esac
	    ;;
    esac
fi

dnl -----------------------------------
dnl checks for other programs

AC_ISC_POSIX
AM_PROG_LEX
AC_PROG_YACC
AC_PROG_AWK
AM_PROG_LIBTOOL
AC_PROG_INSTALL
AC_PROG_LN_S
AC_CHECK_PROG(SED,sed,sed,false)
AC_CHECK_PROG(SORT,sort,sort,false)
AC_CHECK_PROG(KSH,ksh,ksh,sh)

# are we building for windows?
# (these tests must follow AC_ISC_POSIX)
#AC_CYGWIN
#AC_MINGW32

dnl -----------------------------------
dnl where are we in the directory structure

BUILD_DIR=`pwd`
cd ${srcdir}
TOP_DIR=`pwd`
cd ${BUILD_DIR}

AC_SUBST(TOP_DIR)
AC_SUBST(BUILD_DIR)

dnl -----------------------------------
dnl Check for various typedefs and provide substitutes if they don't exist.
                                                                                
AC_TYPE_MODE_T
AC_TYPE_PID_T
AC_TYPE_SIZE_T
AC_TYPE_UID_T

dnl -----------------------------------
dnl old codegens

AC_ARG_ENABLE(codegens,
  [AC_HELP_STRING([--disable-codegens], [don't build old codegens])])
if test "x$enable_codegens" = "xno"; then
  AC_DEFINE(DISABLE_CODEGENS,1,[Define if you don't want the old codegens])
fi
AM_CONDITIONAL(DISABLE_CODEGENS, [test "x$enable_codegens" = "xno"])

dnl -----------------------------------
dnl gvrender plugins

AC_ARG_ENABLE(gvrender,
  [AC_HELP_STRING([--disable-gvrender], [don't build gvrender plugins])])
if test "x$enable_gvrender" = "xno"; then
  AC_DEFINE(DISABLE_GVRENDER,1,[Define if you don't want the new gvrender codegens])
fi
AM_CONDITIONAL(DISABLE_GVRENDER, [test "x$enable_gvrender" = "xno"])

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

AC_CHECK_PROG(SWIG,swig,swig)
HAVE_SWIG=1
if test "x$SWIG" != "x"; then
  SWIG_VERSION=`swig -version 2>&1 | grep Version | cut -d ' ' -f 3`
  SWIG_VERSION_MAJOR=`echo $SWIG_VERSION | cut -d '.' -f 1`
  SWIG_VERSION_MINOR=`echo $SWIG_VERSION | cut -d '.' -f 2`
  if test $SWIG_VERSION_MAJOR -lt 2; then
    if test $SWIG_VERSION_MAJOR -lt 1; then
	HAVE_SWIG=0
    else
      if test $SWIG_VERSION_MINOR -lt 3; then
	HAVE_SWIG=0
      fi
    fi
  fi
fi
AC_SUBST(SWIG_VERSION)
AC_SUBST(HAVE_SWIG)
AM_CONDITIONAL(WITH_SWIG, [test "x$HAVE_SWIG" = "x1"])

dnl -----------------------------------
dnl INCLUDES and LIBS for CSHARP

AC_CHECK_PROG(CSHARP,mcs,mcs)
AM_CONDITIONAL(WITH_CSHARP, [test "x$CSHARP" != "x"])
if test "x$CSHARP" = "x"; then
  HAVE_CSHARP=0
else
  HAVE_CSHARP=1
  CSHARP_INCLUDES=
  CSHARP_LIBS=
fi
AC_SUBST(HAVE_CSHARP)
AC_SUBST(CSHARP_INCLUDES)
AC_SUBST(CSHARP_LIBS)

dnl -----------------------------------
dnl INCLUDES and LIBS for GUILE

AC_CHECK_PROG(GUILE,guile,guile)
if test "x$GUILE" = "x"; then
  HAVE_GUILE=0
else
  HAVE_GUILE=1
  GUILE_INCLUDES=
  GUILE_LIBS=
fi
AC_CHECK_HEADER(libguile.h,,[
  AC_MSG_WARN([Unable to find header libguile.h. The Guile packages will not be built])
  HAVE_GUILE=0])
AM_CONDITIONAL(WITH_GUILE, [test "x$HAVE_GUILE" = "x1"])
AC_SUBST(HAVE_GUILE)
AC_SUBST(GUILE_INCLUDES)
AC_SUBST(GUILE_LIBS)

dnl -----------------------------------
dnl INCLUDES and LIBS for IO

AC_CHECK_PROG(IO,io,io)
AM_CONDITIONAL(WITH_IO, [test "x$IO" != "x"])
if test "x$IO" = "x"; then
  HAVE_IO=0
else
  HAVE_IO=1
  IO_INCLUDES=
  IO_LIBS=
fi
AC_SUBST(HAVE_IO)
AC_SUBST(IO_INCLUDES)
AC_SUBST(IO_LIBS)

dnl -----------------------------------
dnl INCLUDES and LIBS for JAVA

AC_CHECK_PROG(JAVA,java,java)
AM_CONDITIONAL(WITH_JAVA, [test "x$JAVA" != "x"])
if test "x$JAVA" = "x"; then
  HAVE_JAVA=0
else
  HAVE_JAVA=1
  JAVA_INCLUDES=
  JAVA_LIBS=
fi
AC_CHECK_HEADER(jni.h,,[
  AC_MSG_WARN([Unable to find header jni.h. The Java packages will not be built])
  HAVE_JAVA=0])
AC_SUBST(HAVE_JAVA)
AC_SUBST(JAVA_INCLUDES)
AC_SUBST(JAVA_LIBS)

dnl -----------------------------------
dnl INCLUDES and LIBS for OCAML

AC_CHECK_PROG(OCAML,ocaml,ocaml)
AM_CONDITIONAL(WITH_OCAML, [test "x$OCAML" != "x"])
if test "x$OCAML" = "x"; then
  HAVE_OCAML=0
else
  HAVE_OCAML=1
  OCAML_INCLUDES=
  OCAML_LIBS=
fi
AC_SUBST(HAVE_OCAML)
AC_SUBST(OCAML_INCLUDES)
AC_SUBST(OCAML_LIBS)

dnl -----------------------------------
dnl INCLUDES and LIBS for PERL

AC_CHECK_PROG(PERL,perl,perl)
AM_CONDITIONAL(WITH_PERL, [test "x$PERL" != "x"])
if test "x$PERL" = "x"; then
  HAVE_PERL=0
else
  HAVE_PERL=1
  PERL_ARCHLIB=`perl -e 'use Config; print $Config{archlib};'`
  PERL_INCLUDES=-I$PERL_ARCHLIB/CORE
  PERL_LIBS="-L$PERL_ARCHLIB/CORE -lperl"
fi
AC_SUBST(HAVE_PERL)
AC_SUBST(PERL_INCLUDES)
AC_SUBST(PERL_LIBS)

dnl -----------------------------------
dnl INCLUDES and LIBS for PHP

AC_CHECK_PROG(PHP,php,php)
AM_CONDITIONAL(WITH_PHP, [test "x$PHP" != "x"])
if test "x$PHP" = "x"; then
  HAVE_PHP=0
else
  HAVE_PHP=1
  PHP_INCLUDES="-I/usr/include/php -I/usr/include/php/main -I/usr/include/php/Zend -I/usr/include/php/TSRM"
  PHP_LIBS=
fi
AC_SUBST(HAVE_PHP)
AC_SUBST(PHP_INCLUDES)
AC_SUBST(PHP_LIBS)

dnl -----------------------------------
dnl INCLUDES and LIBS for PYTHON

AC_CHECK_PROG(PYTHON,python,python)
if test "x$PYTHON" = "x"; then
  HAVE_PYTHON=0
else
  HAVE_PYTHON=1
  PYTHON_VERSION=`python -V 2>&1 | grep Python | cut -d ' ' -f 2`
  if test "x$PYTHON_VERSION" = "x"; then
    HAVE_PYTHON=0
  else
    PYTHON_VERSION_MAJOR=`echo $PYTHON_VERSION | cut -d '.' -f 1`
    PYTHON_VERSION_MINOR=`echo $PYTHON_VERSION | cut -d '.' -f 2`
    PYTHON_VERSION_SHORT=$PYTHON_VERSION_MAJOR.$PYTHON_VERSION_MINOR
    if test $PYTHON_VERSION_MAJOR -lt 3; then
      if test $PYTHON_VERSION_MAJOR -lt 2; then
	HAVE_PYTHON=0
      else
        if test $PYTHON_VERSION_MINOR -lt 4; then
	  HAVE_PYTHON=0
        fi
      fi
    fi
  fi
fi
AC_CHECK_HEADER(python$PYTHON_VERSION_SHORT/Python.h,,[
  AC_MSG_WARN([Unable to find header Python.h. The Python packages will not be built])
  HAVE_PYTHON=0])
AC_SUBST(PYTHON_VERSION)
AC_SUBST(HAVE_PYTHON)
AM_CONDITIONAL(WITH_PYTHON, [test "x$HAVE_PYTHON" = "x1"])
if test "x$HAVE_PYTHON" = "x1"; then
  PYTHON_INCLUDES=-I/usr/include/python$PYTHON_VERSION_SHORT
  PYTHON_LIBS=-lpython$PYTHON_VERSION_SHORT
fi
AC_SUBST(HAVE_PYTHON)
AC_SUBST(PYTHON_INCLUDES)
AC_SUBST(PYTHON_LIBS)

dnl -----------------------------------
dnl INCLUDES and LIBS for RUBY
AC_CHECK_PROG(RUBY,ruby,ruby)
AM_CONDITIONAL(WITH_RUBY, [test "x$RUBY" != "x"])
if test "x$RUBY" = "x"; then
  HAVE_RUBY=0
else
  HAVE_RUBY=1
  RUBY_INCLUDES=-I`ruby $TOP_DIR/config/config_ruby.rb INCLUDES`
  RUBY_LIBS=-lruby
fi
AC_SUBST(HAVE_RUBY)
AC_SUBST(RUBY_INCLUDES)
AC_SUBST(RUBY_LIBS)

dnl -----------------------------------
dnl INCLUDES and LIBS for TCL

# start by assuming we have it
HAVE_TCL=1 

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
fi

if test "x$TCLSH" = "x"; then
    AC_MSG_WARN([Unable to find a tclsh. The Tcl packages will not be built])
    HAVE_TCL=0
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

if test "$HAVE_TCL" = "1"; then
    TCL_PKGINDEX="gdtclft/pkgIndex.tcl tcldot/pkgIndex.tcl tclpathplan/pkgIndex.tcl gv/pkgIndex.tcl"
else
    TCL_PKGINDEX=""
fi
AC_SUBST(HAVE_TCL)

AM_CONDITIONAL(WITH_TCL, [test "x${HAVE_TCL}" = "x1"])
AC_SUBST(TCL_PKGINDEX)

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
                                                                                
#if test "$HAVE_TK" = "1"; then
#    # TK STUBS support is required (but support is indicated by TCL_SUPPORTS_STUBS)
#    if test "x${TCL_SUPPORTS_STUBS}" != "x1"; then
#        AC_MSG_WARN([Tk does not have STUBs support, perhaps it is too old?  The Tk packages will not be built])
#        HAVE_TK=0
#    fi
#di
                                                                                
if test "$HAVE_TK" = "1"; then
    TK_SUBDIRS="tkstubs tkspline"
    TK_PKGINDEX="tkspline/pkgIndex.tcl"
else
    TK_SUBDIRS=""
    TK_PKGINDEX=""
fi
AC_SUBST(HAVE_TK)
                                                                                
AM_CONDITIONAL(WITH_TK, [test "x${HAVE_TK}" = "x1"])
AC_SUBST(TK_PKGINDEX)

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
#AC_SUBST(TK_LIB_SPEC)
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
dnl Checks for -lm library

save_LIBS=$LIBS
AC_CHECK_LIB(m, main, [MATH_LIBS="-lm"])
AC_SUBST(MATH_LIBS)
LIBS=$save_LIBS

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
EXPAT_LIBDIR="/usr/lib"
AC_ARG_WITH(expatlibdir,
	[  --with-expatlibdir=DIR  use EXPAT libraries from DIR],
	[EXPAT_LIBDIR="$withval"])
if test "$EXPAT_LIBDIR" != "/usr/lib"; then
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
AC_CHECK_HEADERS([X11/Xaw/Text.h], [], [],
   [#if HAVE_X11_INTRINSIC_H
   # include <X11/Intrinsic.h>
   # endif
   ])
AC_CHECK_LIB(Xaw,main,
	[XAW_LIBS="$XAW_LIBS -lXaw"
	AC_DEFINE_UNQUOTED(HAVE_LIBXAW,1,[Define if you have the XAW library])],
		AC_MSG_WARN(Optional Xaw library not available), [-lXmu -lXt -lX11 -lXext $SOCKET_LIBS])
LDFLAGS=$save_LDFLAGS
CPPFLAGS=$save_CPPFLAGS
AC_SUBST(XAW_INCLUDES)
AC_SUBST(XAW_LIBS)

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
dnl INCLUDES and LIBS for GD
                                                                                
AC_ARG_WITH(mylibgd,
  [AC_HELP_STRING([--with-mylibgd],
                  [use internal gd library in preference to any installed libgd])])

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

if test "x$with_mylibgd" != "xyes"; then
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
        AC_MSG_WARN(Optional GD library not available),
	$GD_LIBS),
    AC_MSG_WARN(Optional GD library not available - no gd.h))
    LDFLAGS=$save_LDFLAGS
    CPPFLAGS=$save_CPPFLAGS
else
    # using the internal libgd
    AC_MSG_WARN(Using internal libgd source.)
    AC_DEFINE_UNQUOTED(HAVE_GD_GIF,1,[Define if the GD library has the GD_GIF feature])
    GD_INCLUDES='-I$(top_srcdir)/lib/gd'
    GD_LIBS='$(top_builddir)/lib/gd/libgvgd.la'
fi
AC_SUBST(GD_INCLUDES)
AC_SUBST(GD_LIBS)
                                                                                
AM_CONDITIONAL(WITH_MYLIBGD, [test "x$with_mylibgd" = "xyes"])

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

dnl ----------------------------------
dnl INCLUDES and LIBS for FREETYPE (only with mylibgd)

if test "x$with_mylibgd" = "xyes"; then

  dnl FreeType configure tests snarfed from libgd ..from libwmf ..
  FREETYPE_DIR="yes" 
  AC_ARG_WITH(freetype,
  [  --with-freetype=DIR     where to find the freetype 2.x library],
         FREETYPE_DIR=$withval)

  if test "x$FREETYPE_DIR" = "xno"; then
    AC_MSG_WARN(Optional FREETYPE2 library disabled)
  else
                                                                                
    if test "x$FREETYPE_DIR" != "xyes"; then
      AC_PATH_PROG(FREETYPE_CONFIG,freetype-config,,[$FREETYPE_DIR/bin:$PATH])
    else
      AC_PATH_PROG(FREETYPE_CONFIG,freetype-config)
    fi

    if test -n "$FREETYPE_CONFIG"; then
      if test "x$FREETYPE_DIR" != "xyes"; then
        FT_INCLUDES="`$FREETYPE_CONFIG --cflags` -I$FREETYPE_DIR/include"
      else
        FT_INCLUDES=`$FREETYPE_CONFIG --cflags`
      fi
      ft_libtool=`$FREETYPE_CONFIG --libtool`
      # check that it really exists (FreeBSD apparently forgot to insttall it!)
      if test -f "$ft_libtool"; then
	FT_LIBTOOL="$ft_libtool"
      else
	FT_LIBTOOL=""
      fi
      FT_LIBS=`$FREETYPE_CONFIG --libs`
      FT_LDFLAGS=`echo " $FT_LIBS" |sed 's/ -l[[^ ]][[^ ]]*//g'`
    else
      if test "x$FREETYPE_DIR" != "xyes"; then
        FT_INCLUDES="-I$FREETYPE_DIR/include/freetype2 -I$FREETYPE_DIR/include"
        FT_LDFLAGS="-L$FREETYPE_DIR/lib"
        FT_LIBS="-lfreetype"
      else
        FT_INCLUDES=""
        FT_LDFLAGS=""
        FT_LIBS=""
      fi
      FT_LIBTOOL=""
    fi

    save_CPPFLAGS=$CPPFLAGS
    save_LDFLAGS=$LDFLAGS
    CPPFLAGS="$CPPFLAGS $FT_INCLUDES"
    LDFLAGS="$LDFLAGS $FT_LDFLAGS"
    AC_CHECK_HEADERS(ft2build.h)
    if test `eval echo '${'$as_ac_Header'}'` = yes; then
	AC_CHECK_LIB(freetype,main,
		[FT_LIBS="$FT_LIBTOOL $FT_LIBS"
		AC_DEFINE_UNQUOTED(HAVE_GD_FREETYPE,1,[Define if the GD library has the GD_FREETYPE feature])
		AC_DEFINE_UNQUOTED(HAVE_LIBFREETYPE,1,[Define if you have the FREETYPE2 library])],
		[AC_MSG_WARN(Optional FREETYPE2 library not available - no libfreetype.)])
    else
	AC_MSG_WARN(Optional FREETYPE2 library not available - no ft2build.h)
    fi
    CPPFLAGS=$save_CPPFLAGS
    LDFLAGS=$save_LDFLAGS
    AC_SUBST(FT_INCLUDES)
    AC_SUBST(FT_LIBS)
  fi
fi

dnl ----------------------------------
dnl INCLUDES and LIBS for FONTCONFIG (only with mylibgd)

if test "x$with_mylibgd" = "xyes"; then

save_CPPFLAGS=$CPPFLAGS
save_LDFLAGS=$LDFLAGS
FC_INCLUDES=""
FC_LIBS=""
AC_PATH_PROG(FONTCONFIG_CONFIG,fontconfig-config)
if test -n "$FONTCONFIG_CONFIG"; then
        fc_cflags=`$FONTCONFIG_CONFIG --cflags`
	if test "$fc_cflags" != "-I/usr/include" ; then
		FC_INCLUDES="$fc_cflags"
	fi
	fc_prefix=`$FONTCONFIG_CONFIG --prefix`
	if test "$fc_prefix" != "/usr" ; then
		FC_LIBS="-L${fc_prefix}/lib"
	fi
fi

PKG_CHECK_MODULES(FONTCONFIG, fontconfig,
	[AC_DEFINE_UNQUOTED(HAVE_FONTCONFIG,1,
                 [Define if you have the fontconfig library])],
	[AC_MSG_WARN(Optional fontconfig library not available)])

if test "x$HAVE_FONTCONFIG" = "x1"; then
	FC_INCLUDES=$(FONTCONFIG_INCLUDES)
	FC_LIBS=$(FONTCONFIG_LIBS)
fi

AC_ARG_WITH(fontconfigincludedir,
	[  --with-fontconfigincludedir=DIR use FONTCONFIG includes from DIR],
	FC_INCLUDES="-I$withval")
CPPFLAGS="$CPPFLAGS $FC_INCLUDES"
AC_ARG_WITH(fontconfiglibdir,
	[  --with-fontconfiglibdir=DIR use FONTCONFIG libraries from DIR],
	[FC_LIBS="-L$withval"])
LDFLAGS="$LDFLAGS $FC_LIBS"
AC_CHECK_HEADERS(fontconfig/fontconfig.h)
if test `eval echo '${'$as_ac_Header'}'` = yes; then
	AC_CHECK_LIB(fontconfig,FcGetVersion,
		[FC_LIBS="$FC_LIBS -lfontconfig"
		AC_DEFINE_UNQUOTED(HAVE_GD_FONTCONFIG,1,[Define if the GD library has the GD_FONTCONFIG feature])

		AC_DEFINE_UNQUOTED(HAVE_LIBFONTCONFIG,1,[Define if you have the FONTCONFIG library])],
		[AC_MSG_WARN(Optional FONTCONFIG library not available - no libfontconfig.)])
else
	AC_MSG_WARN(Optional FONTCONFIG library not available - no fontconfig/fontconfig.h)
fi
CPPFLAGS=$save_CPPFLAGS
LDFLAGS=$save_LDFLAGS
AC_SUBST(FC_INCLUDES)
AC_SUBST(FC_LIBS)

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
JPEG_LIBDIR="/usr/lib"
AC_ARG_WITH(jpeglibdir,
	[  --with-jpeglibdir=DIR   use JPEG libraries from DIR],
	[JPEG_LIBDIR="$withval"])
if test "$JPEG_LIBDIR" != "/usr/lib"; then
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

dnl -----------------------------------
dnl INCLUDES and LIBS for cairo

if test "x$enable_gvrender" != "xno"; then
	PKG_CHECK_MODULES(CAIRO, cairo >= 0.1.3,
		[AC_DEFINE_UNQUOTED(HAVE_CAIRO,1,
			[Define if you have the cairo library [EXPERIMENTAL]])],
		[AC_MSG_WARN(Optional cairo library not available [EXPERIMENTAL])])
else
	CAIRO_CFLAGS=""
	CAIRO_LIBS=""
fi
AC_SUBST(CAIRO_CFLAGS)
AC_SUBST(CAIRO_LIBS)

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
	strtoul strtoll strtoull feenableexcept fesetenv])

AC_REPLACE_FUNCS([strcasecmp strncasecmp])

# Check for st_blksize in struct stat
# AC_ST_BLKSIZE

# Internationalization macros
# AM_GNU_GETTEXT

# Checks for header files
# AC_STDC_HEADERS
# AC_HAVE_HEADERS(string.h)
AC_CHECK_HEADERS(stdarg.h stddef.h stddef.h stdlib.h stdint.h malloc.h \
	search.h getopt.h pthread.h values.h float.h limits.h termios.h \
	errno.h time.h sys/time.h sys/times.h sys/types.h unistd.h fenv.h \
	sys/select.h fpu_control.h sys/fpu.h strings.h sys/socket.h sys/stat.h)
AC_HEADER_TIME
AC_HEADER_DIRENT

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
TCLINT_INCLUDES="-I$TOP_DIR/tclpkg/tclstubs"
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
TKINT_INCLUDES="-I$TOP_DIR/tclpkg/tkstubs"
fi
fi
fi
AC_SUBST(TKINT_INCLUDES)

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
if test "x`nm conftest.o |grep __eprintf`" != "x"; then
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
	lib/sfio/Sfio_dc/Makefile
	lib/sfio/Sfio_f/Makefile
	lib/vmalloc/Makefile
	lib/dotgen/Makefile
	lib/dotgen/libdotgen.pc
	lib/neatogen/Makefile
	lib/neatogen/libneatogen.pc
	lib/fdpgen/Makefile
	lib/fdpgen/libfdpgen.pc
	lib/circogen/Makefile
	lib/circogen/libcircogen.pc
	lib/twopigen/Makefile
	lib/twopigen/libtwopigen.pc
	lib/pack/Makefile
	lib/pack/libpack.pc
	lib/expr/Makefile
	lib/common/Makefile
	lib/common/libcommon.pc
	lib/ingraphs/Makefile
	lib/gvc/Makefile
	lib/gvc/libgvc.pc
	plugin/Makefile
	plugin/cairo/Makefile
	plugin/gd/Makefile
	plugin/text/Makefile
	plugin/layout/Makefile
	cmd/Makefile
	cmd/dot/Makefile
	cmd/dot/dotneato-config
	cmd/dot/libdotneato.pc
	cmd/tools/Makefile
	cmd/gvpr/Makefile
        cmd/lefty/ws/x11/libfilereq/Makefile
        cmd/lefty/ws/x11/Makefile
        cmd/lefty/ws/Makefile
        cmd/lefty/os/unix/Makefile
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
	ast_common.h
	)

AC_OUTPUT
