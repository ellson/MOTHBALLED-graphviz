#! /bin/sh

if ! GRAPHVIZ_GIT_DATE=$( git log -n 1 --format=%ci ) ; then
    GRAPHVIZ_VERSION_DATE="0"
    echo "Warning: build not started in a Git clone, or Git is not installed: setting version date to 0." >&2
else
    if ! GRAPHVIZ_VERSION_DATE=$( date -u +%Y%m%d.%H%M -d "$GRAPHVIZ_GIT_DATE" 2>/dev/null ) ; then
        # try date with FreeBSD syntax
        if ! GRAPHVIZ_VERSION_DATE=$( date -u -j -f "%Y-%m-%d %H:%M:%S %z" "$GRAPHVIZ_GIT_DATE" +%Y%m%d.%H%M ); then
            echo "Warning: we do not know how to invoke date correctly." >&2
        fi    
    fi
    echo "Graphviz: version date is based on time of last commit: $GRAPHVIZ_VERSION_DATE"

    GRAPHVIZ_VERSION_COMMIT=$( git log -n 1 --format=%h )
    echo "Graphviz: abbreviated hash of last commit: $GRAPHVIZ_VERSION_COMMIT"
fi

# initialize version for a "stable" build
cat >./version.m4 <<EOF
dnl Graphviz package version number, (as distinct from shared library version)
dnl For the minor number: odd => unstable series
dnl                       even => stable series
dnl For the micro number: 0 => in-progress development
dnl                       timestamp => tar-file snapshot or release
m4_define(graphviz_version_major, 2)
m4_define(graphviz_version_minor, 40)
m4_define(graphviz_version_micro, 1)
dnl NB: the next line gets changed to a date/time string for development releases
dnl m4_define(graphviz_version_micro, $GRAPHVIZ_VERSION_DATE)
m4_define(graphviz_version_date, $GRAPHVIZ_VERSION_DATE)
m4_define(graphviz_collection, development)
m4_define(graphviz_version_commit, $GRAPHVIZ_VERSION_COMMIT)
EOF

autoreconf -v --install --force || exit 1

# ensure config/depcomp exists even if still using automake-1.4
# otherwise "make dist" fails.
touch config/depcomp

# suppress automatic ./configure  is "./autogen.sh NOCONFIG"
if test "$1" != "NOCONFIG"; then
    # don't use any old cache, but create a new one
    rm -f config.cache
    ./configure -C "$@"
fi
