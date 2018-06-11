#!/bin/bash -e

cd "$(dirname ${BASH_SOURCE[0]})/../.."

QMAKESPECPARAMS=
CLEAN=false
DEBUGRELEASE=release
PBUILDER=false
JOBS=1
TIMEOUT=360
LOCALCONFIG="tools/jenkins-linux-localconfig.pri"

parsecmd() {
    while [ $# -gt 0 ]; do
        #CMDSTART
        case "$1" in
        --spec) # [linux-clang|...]
            # use a specific qmake build spec
            QMAKESPECPARAMS="-spec $2"
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
        --pbuilder) #
            # run the build in a pbuilder environment
            PBUILDER=true
            ;;
        --no-pbuilder) #
            # do not run the build in a pbuilder environment
            PBUILDER=false
            ;;
        -j|--jobs) # [jobs]
            # specifies the number of jobs to run simultaneously
            JOBS=$2
            shift
            ;;
        -t|--timeout) # [SECONDS]
            # use a custom timeout instead of the standard 800s
            TIMEOUT=$2
            shift
            ;;
        --local-config) #
            # supply a custom localconfig
            LOCALCONFIG=$2
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

if [ "$PBUILDER" = "true" ]; then
    echo '#!/bin/sh -e' > "$WORKSPACE/pbuilder-run.sh"
    # the escaping is wrong, but this shouldn't matter for the arguments that we care about
    echo "su jenkins -c '$WORKSPACE/common/tools/jenkins-linux-build.sh $@ --no-pbuilder'" >> "$WORKSPACE/pbuilder-run.sh"
    chmod +x "$WORKSPACE/pbuilder-run.sh"
    find "$WORKSPACE" -type d -exec chmod g+s {} \;
    sudo pbuilder execute --bindmounts ~jenkins -- "$WORKSPACE/pbuilder-run.sh"
    exit 0
fi

if [ "$CLEAN" = "true" ]; then
    rm -rf bin .build
fi

cp "$LOCALCONFIG" localconfig.pri

rm -f *test-results.xml *.memcheck

export QT_SELECT=qt5

if [ "$DEBUGRELEASE" = "release" ]; then
    qmake $QMAKESPECPARAMS RELEASE=1
else
    qmake $QMAKESPECPARAMS RELEASE=
fi
make qmake_all -j "$JOBS"
make -j "$JOBS"

timeout "$TIMEOUT" make testxml
