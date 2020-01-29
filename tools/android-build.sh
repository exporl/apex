#!/bin/bash -e

# TODO: This script should move completely to qmake and clebs so that Android
# builds only need sth. like
#   cp tools/jenkins-android-localconfig.pri localconfig.pri
#   qmake
#   make
#   make install
#   make deploy

cd "$(dirname $(readlink -f ${BASH_SOURCE[0]}))/.."

ROOTDIR=$(pwd)
APIDIR=$ROOTDIR/.build/api-android
CLEAN=false
DEBUGRELEASE=debug
QMAKEARGS=
ANDROIDDEPLOYQTARGS=
BUILD_APK=false
BUILD_TYPE=armv7
ANDROID_API_LEVEL=
JOBS=1
VERSION_CODE=$(git show -s --format="%ct" HEAD)
VERSION_NAME=$(git describe --always --tags --dirty)
KEYSTORE=
KEYSTORE_PWD_PATH=

source tools/android-versions.sh

parsecmd() {
    while [ $# -gt 0 ]; do
        #CMDSTART
        case "$1" in
        --api-dir) #
            # the directory containing all the tools needed to compile
            APIDIR=${2%/}
            shift
            ;;
        -c|--clean) #
            # clean the build tree before building
            CLEAN=true
            ;;
        --build-apk) #
            # attempts to build an apk
            BUILD_APK=true
            ;;
        --x86) #
            # builds the x86 version instead of armv7
            BUILD_TYPE=x86
            ;;
        -r|--release) #
            # build an unsigned release package, expects --build-apk to be true
            DEBUGRELEASE=release
            QMAKEARGS="RELEASE=1"
            ANDROIDDEPLOYQTARGS=--release
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
        --android-api-level) # [16|...]
            # use a specific android platform target instead of the default one
            ANDROID_API_LEVEL=$2
            shift
            ;;
        -j|--jobs) # [jobs]
            # specifies the number of jobs to run simultaneously
            JOBS=$2
            shift
            ;;
        -vc|--version-code) #
            # specifies the version code, should be a natural number
            VERSION_CODE=$2
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

if [[ ! $VERSION_CODE =~ ^-?[0-9]+$ ]]; then
    echo "Version code $VERSION_CODE is not a natural number"
    exit 1
fi

if [ "$CLEAN" = "true" ]; then
    rm -rf .build/android-$DEBUGRELEASE .build/android-$DEBUGRELEASE-installed
    rm -rf bin/android-$DEBUGRELEASE bin/android-$DEBUGRELEASE-installed
    find -name "android-*.so-deployment-settings.json" -delete -o -name "Makefile" -delete
fi

PREFIX=$APIDIR/host/$BUILD_TYPE/usr
QT_ROOT=$APIDIR/Qt/$QT_VERSION/android_$BUILD_TYPE
INSTALLDIR=$ROOTDIR/bin/android-$DEBUGRELEASE-installed/$BUILD_TYPE/apex
export ANDROID_SDK_ROOT=$APIDIR/android-sdk-linux
export ANDROID_NDK_ROOT=$APIDIR/android-ndk-${ANDROID_NDK_VERSION}
export PKG_CONFIG_LIBDIR=$PREFIX/lib/pkgconfig
export PKG_CONFIG_SYSROOT_DIR=/

# Build Android manifest
rm -rf $ROOTDIR/.build/android-$DEBUGRELEASE/template
mkdir -p $ROOTDIR/.build/android-$DEBUGRELEASE/template/src/be/kuleuven/med/exporl/apex
MANIFEST_TESTS=
for i in $ROOTDIR/*/tests/*/*.pro; do
    TARGET=$(grep TARGET "$i" | sed 's/.*=//;s/ //g')
    if [ -z "$TARGET" ]; then
        TARGET=$(basename "$i" .pro)
    fi
    sed "s/-- %%INSERT_TEST_ACTIVITY%% --/${TARGET^}Activity/g" \
        "$ROOTDIR/tools/android-template/TestActivity.java.in" \
        > "$ROOTDIR/.build/android-$DEBUGRELEASE/template/src/be/kuleuven/med/exporl/apex/${TARGET^}Activity.java"
    MANIFEST_TESTS="$MANIFEST_TESTS$(cat "$ROOTDIR/tools/android-template/AndroidManifest.test.xml.in" \
        | sed "s/-- %%INSERT_TEST_NAME%% --/${TARGET}/g;s/-- %%INSERT_TEST_ACTIVITY%% --/${TARGET^}Activity/g" \
        | sed ':a;N;$!ba;s/\n/\\n/g')"
done

sed "s#-- %%INSERT_TEST_TEMPLATES%% --#$MANIFEST_TESTS#" "$ROOTDIR/tools/android-template/AndroidManifest.xml.in" \
    | sed "s#-- %%INSERT_VERSION_NAME%% --#$VERSION_NAME#" \
    | sed "s#-- %%INSERT_VERSION_CODE%% --#$VERSION_CODE#" \
    | sed "s#-- %%INSERT_MIN_SDK_VERSION%% --#$ANDROID_API_LEVEL#" \
    | sed "s#-- %%INSERT_TARGET_SDK_VERSION%% --#$ANDROID_TARGET_SDK_LEVEL#" \
    > "$ROOTDIR/.build/android-$DEBUGRELEASE/template/AndroidManifest.xml"

cp -r $ROOTDIR/src/java/* $ROOTDIR/.build/android-$DEBUGRELEASE/template/src/be/kuleuven/med/exporl/apex/
cp -r $ROOTDIR/tools/android-template/res $ROOTDIR/.build/android-$DEBUGRELEASE/template/

# Build source tree
$QT_ROOT/bin/qmake -spec android-clang $QMAKEARGS \
    "ANDROID_PACKAGE_SOURCE_DIR=$ROOTDIR/.build/android-$DEBUGRELEASE/template" \
    "ANDROID_EXTRA_LIBS += \
        $PREFIX/lib/libxml2.so \
        $PREFIX/lib/libportaudio.so \
        $PREFIX/lib/libsndfile.so \
        $PREFIX/lib/libfftw3f.so \
        $PREFIX/lib/libfftw3.so \
        $PREFIX/lib/libgit2.so \
        $PREFIX/lib/libcrypto_1_1.so \
        $PREFIX/lib/libssl_1_1.so \
        $PREFIX/lib/libssh2.so \
        $PREFIX/lib/libgit2.so \
        $ROOTDIR/bin/android-$DEBUGRELEASE/libapexmain.so"

make -j "$JOBS" qmake_all
make -j "$JOBS"
make -j "$JOBS" install "INSTALL_ROOT=$INSTALLDIR"

if [ "$BUILD_APK" = "true" ]; then
    if [ "$DEBUGRELEASE" = "release" ] && ([ -z "$KEYSTORE" ] || [ -z "$KEYSTORE_PWD_PATH" ]); then
        echo "Specify --ks and --ks-pass-path to sign the package" && exit 1
    fi

    if [ "$DEBUGRELEASE" = "release" ]; then
        ANDROIDDEPLOYQTARGS="$ANDROIDDEPLOYQTARGS --sign $KEYSTORE apex-release --storepass $(cat $KEYSTORE_PWD_PATH)"
    fi

    mkdir -p $INSTALLDIR/assets/doc && git show --stat > $INSTALLDIR/assets/doc/commit.txt

    $QT_ROOT/bin/androiddeployqt --output $INSTALLDIR --input "$ROOTDIR/src/programs/apex/android-libapex.so-deployment-settings.json" --gradle --android-platform android-${ANDROID_API_LEVEL} $ANDROIDDEPLOYQTARGS

    if [ $DEBUGRELEASE = "release" ];then
        APK_FILENAME_SUFFIX="-signed"
    else
        APK_FILENAME_SUFFIX=
    fi

    echo "To install do: adb install -r ${INSTALLDIR}/build/outputs/apk/${DEBUGRELEASE}/apex-${DEBUGRELEASE}${APK_FILENAME_SUFFIX}.apk"
fi
