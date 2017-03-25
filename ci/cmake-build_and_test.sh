#!/bin/sh

# Run this script from the root source directory

mkdir build || true

cd build

if cmake .. ; then
    echo "'cmake ..' succesfull."
else
    echo "Error: 'cmake ..' failed." >&2
    exit 1
fi

if cmake --build . ; then
    echo "'cmake --build .' succesfull."
else
    echo "Error: 'cmake --build .' failed." >&2
    exit 1
fi

if cpack ; then
    echo "'cpack' succesfull."
else
    echo "Error: 'cpack' failed." >&2
    exit 1
fi

if make install ; then
    echo "'make install' succesfull."
else
    echo "Error: 'make install' failed." >&2
    exit 1
fi

# Not all platforms include this directory when running ldconfig, the CentOS
# docker image for example.
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/usr/local/lib

if ldconfig ; then
    echo "'ldconfig' succesfull."
else
    echo "Error: 'ldconfig' failed." >&2
    exit 1
fi

if dot -c ; then
    echo "'dot -c' succesfull."
else
    echo "Error: 'dot -c' failed." >&2
    exit 1
fi

if ctest ; then
    echo "'ctest' succesfull."
else
    echo "Error: 'ctest' failed." >&2
    exit 1
fi
