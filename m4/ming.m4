# Adapted for graphviz by John Ellson, Oct/24/2006

# Configure paths for Ming
# Sandro Santilli 2006-01-24
#
# This macro uses ming-config, which was
# not available as for Ming 0.3beta1

# Use: 
#	GV_PATH_MING
#
# Provides:
#	MING_CFLAGS
#	MING_LIBS
#       MAKESWF

AC_DEFUN([GV_PATH_MING],
[
	MING_CFLAGS=""
	MING_LIBS=""
	WITH_MING=yes

	AC_ARG_WITH(ming, AC_HELP_STRING([--with-ming=[<ming-config>]], [Use ming to build tests]),
		[
		case "${withval}" in
			yes)	WITH_MING=yes
				;;
			no)	WITH_MING=no
				;;
			*)	WITH_MING=yes
				MING_CONFIG=${withval}
				;;
		esac
		], MING_CONFIG="")

	if test x"$MING_CONFIG" = "x"; then
		AC_PATH_PROG(MING_CONFIG, ming-config)
	fi

	if test x"$MING_CONFIG" != "x" -a "$WITH_MING" = "yes"; then
		MING_CFLAGS=`$MING_CONFIG --cflags`
		MING_LIBS=`$MING_CONFIG --libs`
                MING_PATH=`$MING_CONFIG --bindir`
                AC_PATH_PROG([MAKESWF], [makeswf], , [$PATH:$MING_PATH])
	fi

	AC_SUBST(MING_CFLAGS)
	AC_SUBST(MING_LIBS)
	AC_SUBST(MAKESWF)
])

