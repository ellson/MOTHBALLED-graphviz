#!/bin/sh

# Run this script from the root source directory 

if ./autogen.sh ; then
    echo "autogen.sh succesfull."
else
    echo "Error: autogen.sh failed." >&2
    exit 1
fi

if make ; then
    echo "make succesfull."
else
    echo "Error: make failed." >&2
    exit 1
fi

if make install ; then
    echo "make install succesfull."
else
    echo "Error: make install failed." >&2
    exit 1
fi

if make check ; then
    echo "make check succesfull."
else
    echo "Error: make check failed." >&2
    exit 1
fi

if make dist ; then
    echo "make dist succesfull."
else
    echo "Error: make dist failed." >&2
    exit 1
fi
