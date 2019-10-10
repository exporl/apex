#!/bin/bash -e

cd "$(dirname ${BASH_SOURCE[0]})/../.."

CLEAN=false
DEBUGRELEASE=release
JOBS=1
SPEC=

parsecmd() {
    while [ $# -gt 0 ]; do
        #CMDSTART
        case "$1" in
        --spec) # [linux-clang]
            # use a specific compiler
            SPEC=$2
            shift
            ;;
        -c|--clean) #
            # clean the build tree beforehand
            CLEAN=true
            ;;
        -b|--build) # [release|debug]
            # debug: build with debug settings
            # release: build with release settings
            DEBUGRELEASE=$2
            shift
            ;;
        -j|--jobs) # [jobs]
            # specifies the number of jobs to run simultaneously
            JOBS=$2
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

if [ "$CLEAN" = "true" ]; then
    rm -rf bin cmake-build
fi

rm -f *test-results.xml

mkdir cmake-build
pushd cmake-build

if [ "$SPEC" = "linux-clang" ]; then
    CMAKE_C_COMPILER_OPTION="-DCMAKE_C_COMPILER=/usr/bin/clang"
    CMAKE_CXX_COMPILER_OPTION="-DCMAKE_CXX_COMPILER=/usr/bin/clang++"
fi

if [ "$DEBUGRELEASE" = "release" ]; then
    cmake -DCMAKE_BUILD_TYPE=Release "$CMAKE_C_COMPILER_OPTION" "$CMAKE_CXX_COMPILER_OPTION" ..
else
    cmake -DCMAKE_BUILD_TYPE=Debug "$CMAKE_C_COMPILER_OPTION" "$CMAKE_CXX_COMPILER_OPTION" ..
fi

cmake --build . -j "$JOBS"
ctest -j "$JOBS" .

popd