#!/bin/sh -e

cd `dirname $0`
cd ..

ROOTDIR=$(pwd)
EXTRADIR=false

parsecmd() {
    while [ $# -gt 0 ]; do
        #CMDSTART
        case "$1" in
        -e|--extra) #
            # The directory containing all the tools needed to compile. Default is .build/android
            EXTRADIR=$2
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
            echo "Unknown parameter $1"
            exit 1
            ;;
        esac
        #CMDEND
        shift
    done
}

parsecmd "$@"

cp tools/jenkins-android-localconfig.pri localconfig.pri

if [ $EXTRADIR = false ]; then
    tools/android-prebuild.sh
    tools/android-build.sh
    exit
fi

tools/android-build.sh --extra $EXTRADIR
