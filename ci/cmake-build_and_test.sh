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
