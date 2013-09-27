#!/bin/sh

# FOR USE ONLY DURING NIGHTLY DEVELOPMENT BUILDS

# DO NOT COMMIT TO CVS AFTER RUNNING THIS SCRIPT

# This script is intended for use only during the preparation of master sources for nightly snapshot builds.
# It modifies configure.ac to use an extended package version number containing the current date.
#
# This style of version number is particular to graphviz, and probably wouldn't be done this way if
# we were using SVN rather than CVS.  Also, it has obscure complications when we try to extend it to the Windows platform.
#
# For these reasons, we maintain this custom script here rather than in the generic package build scripts.
#
# This script should not be run for stable builds, and for development builds it should be run first after checkout:
#
#	cvs co graphviz2
#	cd graphviz2
#	./set_dev_version.sh
#	./autogen.sh
#	make dist
#
# John Ellson <ellson@research.att.com>

DATE=`date -u +%Y%m%d.%H%M`

VERSION_MAJOR=`grep 'm4_define(graphviz_version_major' configure.ac | sed 's/.*, \([0-9]*\))/\1/'`
VERSION_MINOR=`grep 'm4_define(graphviz_version_minor' configure.ac | sed 's/.*, \([0-9]*\))/\1/'`
VERSION_MICRO=`grep 'm4_define(graphviz_version_micro' configure.ac | sed 's/.*, \([0-9.]*\))/\1/'`

VERSION_MICRO=$DATE
sed "s/\(m4_define(graphviz_version_micro, \)[0-9.]*)/\1$VERSION_MICRO)/" <configure.ac >t$$
mv t$$ configure.ac
sed "s/\(GRAPHVIZ_COLLECTION\)=.*/\1=development/" <configure.ac >t$$
mv t$$ configure.ac

VERSION=$VERSION_MAJOR.$VERSION_MINOR.$VERSION_MICRO

VERSION_DATE=$DATE
sed "s/VERSION_DATE=.*/VERSION_DATE=$VERSION_DATE/" <configure.ac >t$$
mv t$$ configure.ac













