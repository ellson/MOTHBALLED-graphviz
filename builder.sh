#!/usr/bin/ksh

# build binary products all the way from git

cd $( dirname $0 )

if ! test -d .git; then
    echo "must be run from the top directory of a git clone" >&2 
    exit 1
fi

count=0
git status -s |
while read rec; do
	((count += 1))
done
if test $count -ne 0; then 
    echo "you have uncommitted changes in this directory, quitting" >&2
    exit 1
fi

HOST=`hostname`

# $HOME/.rpmmacros must match this
RPMBUILD=$HOME/rpmbuild/$HOST
rm -rf $RPMBUILD
mkdir -p $RPMBUILD/SOURCES

# get lastest vanilla sources
git clean -xdf
git reset --hard
git pull

git log -1 --date raw |
while read a b c; do
    case "$a" in
    commit) VERSION_COMMIT=$b;;
    Date:) EPOCHSEC=$b; TZ=$c; break;;
    *) continue;;
    esac
done

./autogen.sh NOCONFIG >/dev/null

VERSION_DATE=$( date -d @$EPOCHSEC -u +%Y%m%d.%H%M )
VERSION_MAJOR=`grep 'm4_define(graphviz_version_major' version.m4 | sed 's/.*, \([0-9]*\))/\1/'`
VERSION_MINOR=`grep 'm4_define(graphviz_version_minor' version.m4 | sed 's/.*, \([0-9]*\))/\1/'`
VERSION=$VERSION_MAJOR.$VERSION_MINOR.$VERSION_DATE

if test $(( $VERSION_MINOR % 2 )) -eq 0; then
    VERSION_COLLECTION="stable"
else
    VERSION_COLLECTION="development"
fi

sed "s/\(m4_define(graphviz_version_micro, \).*)/\1$VERSION_DATE)/" <version.m4 >t$$
mv t$$ version.m4
sed "s/\(m4_define(graphviz_collection, \).*)/\1$VERSION_COLLECTION)/" <version.m4 >t$$
mv t$$ version.m4
sed "s/\(m4_define(graphviz_version_date, \).*)/\1$VERSION_DATE)/" <version.m4 >t$$
mv t$$ version.m4
sed "s/\(m4_define(graphviz_version_commit, \).*)/\1$VERSION_COMMIT)/" <version.m4 >t$$
mv t$$ version.m4

./configure >/dev/null

make dist >/dev/null

if ! test -f graphviz-$VERSION.tar.gz; then
    echo "Error: no graphviz-$VERSION.tar.gz was created"
    exit 1
fi

md5sum graphviz-$VERSION.tar.gz >graphviz-$VERSION.tar.gz.md5
tar cf - rtest | gzip >rtest.tar.gz

rpmbuild -ts -D "distroagnostic 1" graphviz-$VERSION.tar.gz || exit 1

rpmbuild --rebuild $RPMBUILD/SRPMS/graphviz-$VERSION-1.src.rpm || exit 1

cp graphviz-$VERSION.tar.gz graphviz-$VERSION.tar.gz.md5 rtest.tar.gz $RPMBUILD/SOURCES/

(cd $RPMBUILD; createrepo .)
