#!/bin/bash -e

cd "$(dirname $(readlink -f ${BASH_SOURCE[0]}))/.."

git submodule update --force --init --remote common
common/tools/jenkins-linux-git-setup.sh

ROOTDIR=$(pwd)
APIDIR=~jenkins/apex-android-extra-folder-qt5.13
CLEAN=
TESTS=true
DEBUGRELEASE=debug
KEYSTORE=
KEYSTORE_PWD_PATH=

parsecmd() {
    while [ $# -gt 0 ]; do
        #CMDSTART
        case "$1" in
        --api-dir) #
            # The directory containing all the tools needed to compile.
            # If this directory does not exist, the android-prepare-api.sh script will be run.
            APIDIR=${2%/}
            shift
            ;;
        -c|--clean) #
            # Cleans the apex project, this does not clean the extra directory
            CLEAN=--clean
            ;;
        --no-runtests) #
            # Don't run the android tests. Needs an armv7 virtual device
            TESTS=false
            ;;
        -r|--release) #
            # build an unsigned release package
            # implies --ks and --ks-pass-path
            DEBUGRELEASE=release
            ;;
        --ks) #
            # path to keystore, see apksigner tool for help
            KEYSTORE=${2%/}
            shift
            ;;
        --ks-pass-path) #
            # path to file with the password for the keystore, see apksigner tool for help
            KEYSTORE_PWD_PATH=${2%/}
            shift
            ;;
        -h|--help) #
            # this help
            echo "Usage: $0 [OPTION]..."
            printf "\nCommand line arguments:\n"
            sed -rn '/CMDSTART/,/CMDEND/{/\) \#|^ +# /{s/\)? #//g;s/^    //;p}}' "$0"
            exit 0
            ;;
        *)
            echo "Warning: ignoring unknown parameter $1"
            exit 1
            ;;
        esac
        #CMDEND
        shift
    done
}

parsecmd "$@"

echo "API directory: $APIDIR"
echo "Clean arguments: $CLEAN"

tools/android-prepare-api.sh --api-dir $APIDIR

if [ "$DEBUGRELEASE" = "release" ];then
    cp tools/jenkins-android-release-localconfig.pri localconfig.pri
    tools/android-build.sh --api-dir "$APIDIR" --build-apk $CLEAN \
                           --release --ks "$KEYSTORE" --ks-pass-path "$KEYSTORE_PWD_PATH"
else
    cp tools/jenkins-android-localconfig.pri localconfig.pri
    tools/android-build.sh --api-dir "$APIDIR" --build-apk $CLEAN
fi

if [ "$TESTS" = "true" ]; then
    rm -rf *test-results.{xml,txt}
    tools/android-test.sh --apk "$ROOTDIR/bin/android-debug-installed/armv7/apex/build/outputs/apk/$DEBUGRELEASE/apex-$DEBUGRELEASE.apk"
fi
