#!/bin/sh

# Run this script from the root source directory 

./autogen.sh
make
make install
make check
make dist
