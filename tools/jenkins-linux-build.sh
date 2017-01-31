#!/bin/sh -e

cp tools/jenkins-linux-localconfig.pri localconfig.pri

rm -f *test-results.xml

if [ "$1" = "clean" ]; then
    rm -rf bin .build
fi

export QT_SELECT=qt5
qmake -v

if [ "$releasetype" = "release" ]; then
    qmake RELEASE=1
else
    qmake RELEASE=
fi
make qmake_all
make

mkdir -p bin/$releasetype/doc
git show --stat > bin/$releasetype/doc/commit.txt

make testxml

tools/jenkins-linux-check.sh
