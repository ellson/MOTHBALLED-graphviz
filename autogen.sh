#! /bin/sh

# autoregen doesn't run libtoolize with --ltdl on older systems, so force it
if test -x /usr/bin/libtoolize; then
    /usr/bin/libtoolize --copy --force --ltdl
elif test -x /opt/local/bin/glibtoolize; then
    # Mac OSx is such a mess!!!
    /opt/local/bin/glibtoolize --copy --force --ltdl
elif test -x /usr/bin/glibtoolize; then
    /usr/bin/glibtoolize --copy --force --ltdl
fi

autoreconf -v --install --force || exit 1

# ensure config/depcomp exists even if still using automake-1.4
# otherwise "make dist" fails.
touch config/depcomp

#create an empty config/config.rpath else iconf configure test generates a warning
touch config/config.rpath

# ensure COPYING is based on cpl1.0.txt
#   cpl1.0.txt was obtained from: http://www.opensource.org/licenses/cpl1.0.txt
rm -f COPYING
cp cpl1.0.txt COPYING

# don't use any old cache, but create a new one
rm -f config.cache
./configure -C "$@"
