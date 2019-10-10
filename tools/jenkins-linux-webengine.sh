#!/bin/bash -e

cd "$(dirname $(readlink -f ${BASH_SOURCE[0]}))/.."

if [ ! -f /var/cache/pbuilder/base.tgz ]; then
    sudo pbuilder create --distribution "bionic" \
         --architecture amd64 \
         --othermirror "deb http://archive.ubuntu.com/ubuntu/bionic main universe"
    echo '#!/bin/sh -e' > "$WORKSPACE/pbuilder-setup.sh"
    echo 'adduser --gecos "" --uid $(id -g jenkins) --disabled-login jenkins' \
         > "$WORKSPACE/pbuilder-setup.sh"
    sudo pbuilder execute --save-after-login -- "$WORKSPACE/pbuilder-setup.sh"
fi

sudo pbuilder update --extrapackages "qtbase5-dev-tools qt5-default qt5-qmake  \
         portaudio19-dev libxml2-dev chrpath libqt5widgets5 libqt5scripttools5 \
         libqt5xmlpatterns5 libqt5xmlpatterns5-dev qtscript5-dev \
         libqt5svg5-dev qtwebengine5-dev libqt5webenginecore5 \
         libqt5webenginewidgets5 qtmultimedia5-dev python-protobuf \
         protobuf-compiler libprotobuf-dev xvfb liblo-dev libsndfile1-dev \
         python-soundfile python-numpy libqt5websockets5-dev \
         libgit2-dev libfftw3-dev libssl-dev"

git submodule update --force --init --remote common
common/tools/jenkins-linux-git-setup.sh
exec common/tools/jenkins-linux-build.sh \
     --pbuilder \
     --local-config "$WORKSPACE/tools/jenkins-linux-webengine-localconfig.pri" \
     "$@"
