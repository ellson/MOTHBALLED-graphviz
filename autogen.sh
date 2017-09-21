#! /bin/sh

if ! GRAPHVIZ_GIT_DATE=$( git log -n 1 --format=%ci ) ; then
    GRAPHVIZ_VERSION_DATE="0"
    echo "Warning: build not started in a Git clone, or Git is not installed: setting version date to 0." >&2
else
    GRAPHVIZ_AUTHOR_NAME=$( git log -n 1 --format="%an" )
    GRAPHVIZ_AUTHOR_EMAIL=$( git log -n 1 --format="%ae" )
    if ! GRAPHVIZ_VERSION_DATE=$( date -u +%Y%m%d.%H%M -d "$GRAPHVIZ_GIT_DATE" 2>/dev/null ) ; then
        # try date with FreeBSD syntax
        if ! GRAPHVIZ_VERSION_DATE=$( date -u -j -f "%Y-%m-%d %H:%M:%S" "$GRAPHVIZ_GIT_DATE" "+%Y%m%d.%H%M" 2>/dev/null); then
            echo "Warning: we do not know how to invoke date correctly." >&2
        fi    
    fi
    if ! GRAPHVIZ_CHANGE_DATE=$( date -u +"%a %b %e %Y" -d "$GRAPHVIZ_GIT_DATE" 2>/dev/null ) ; then
        # try date with FreeBSD syntax
        if ! GRAPHVIZ_CHANGE_DATE=$( date -u -j -f "%Y-%m-%d %H:%M:%S"  "$GRAPHVIZ_GIT_DATE" "+%a %b %e %Y" 2>/dev/null); then
            echo "Warning: we do not know how to invoke date correctly." >&2
        fi    
    fi
    echo "Graphviz: version date is based on time of last commit: $GRAPHVIZ_VERSION_DATE"

    GRAPHVIZ_VERSION_COMMIT=$( git log -n 1 --format=%h )
    echo "Graphviz: abbreviated hash of last commit: $GRAPHVIZ_VERSION_COMMIT"
fi

# initialize version for a "development" build
cat >./version.m4 <<EOF
dnl Graphviz package version number, (as distinct from shared library version)
dnl For the minor number: odd => unstable series
dnl                       even => stable series
dnl For the micro number: 0 => in-progress development
dnl                       timestamp => tar-file snapshot or release
m4_define([graphviz_version_major],[2])
m4_define([graphviz_version_minor],[41])

dnl NB: The next line is for hard-coding the micro version for stable releases
dnl m4_define([graphviz_version_micro],[1])

dnl NB: the next line gets changed to a date/time string for development releases
m4_define([graphviz_version_micro],[$GRAPHVIZ_VERSION_DATE])

m4_define([graphviz_version_date],[$GRAPHVIZ_VERSION_DATE])
m4_define([graphviz_change_date],["$GRAPHVIZ_CHANGE_DATE"])
m4_define([graphviz_git_date],["$GRAPHVIZ_GIT_DATE"])
m4_define([graphviz_author_name],["$GRAPHVIZ_AUTHOR_NAME"])
m4_define([graphviz_author_email],[$GRAPHVIZ_AUTHOR_EMAIL])
m4_define([graphviz_collection],[development])
m4_define([graphviz_version_commit],[$GRAPHVIZ_VERSION_COMMIT])

EOF

# config/missing is created by autoreconf,  but apparently not recreated if already there.
# This breaks some builds from the graphviz.tar.gz sources.
# Arguably this is an autoconf bug.
rm -f config/missing

autoreconf -v --install --force || exit 1

# ensure config/depcomp exists even if still using automake-1.4
# otherwise "make dist" fails.
touch config/depcomp

# suppress automatic ./configure  if "./autogen.sh NOCONFIG"
if test "$1" != "NOCONFIG"; then
    # don't use any old cache, but create a new one
    rm -f config.cache
    ./configure -C "$@"
fi
