#! /bin/sh

autoreconf -v --install --force || exit 1

# ensure config/depcomp exists even if still using automake-1.4
# otherwise "make dist" fails.
touch config/depcomp

#create an empty config/config.rpath else iconf configure test generates a warning
touch config/config.rpath

patch -N -p0 <ltmain.sh.patch
                                                                                
# ensure COPYING is based on LICENSE.html
rm -f COPYING
lynx -dump CPL.html >COPYING

#./configure "$@"

echo
echo "Now type './configure'"
