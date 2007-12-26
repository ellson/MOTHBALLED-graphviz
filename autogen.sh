#! /bin/sh

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
