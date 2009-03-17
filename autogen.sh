#! /bin/sh

# autoregen doesn't run libtoolize with --ltdl on older systems, so force it

LIBTOOLIZE=libtoolize
if test -x /opt/local/bin/glibtoolize; then
    LIBTOOLIZE=/opt/local/bin/glibtoolize
fi
export LIBTOOLIZE

echo "autogen.sh: running: $LIBTOOLIZE --copy --force --ltdl"
$LIBTOOLIZE --copy --force --ltdl

autoreconf -v --install --force || exit 1

# ensure config/depcomp exists even if still using automake-1.4
# otherwise "make dist" fails.
touch config/depcomp

# ensure COPYING is based on cpl1.0.txt
#   cpl1.0.txt was obtained from: http://www.opensource.org/licenses/cpl1.0.txt
rm -f COPYING
cp cpl1.0.txt COPYING

# don't use any old cache, but create a new one
rm -f config.cache
./configure -C "$@"
