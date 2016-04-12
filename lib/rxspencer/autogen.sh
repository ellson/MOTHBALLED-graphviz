#!/bin/sh
libtoolize -c --automake --force
aclocal
autoconf -Wall --force
automake -Wall --add-missing --force-missing --copy
