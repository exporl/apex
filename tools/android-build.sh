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
ANDROID_SDK_ROOT=false
CLEAN=false
DEBUGRELEASE=debug
QMAKEARGS=
ANDROIDDEPLOYQTARGS=
BUILD_APK=false
BUILD_TYPE=armv7
ANDROID_PLATFORM=android-14
JOBS=1
VERSION_CODE=$(git show -s --format="%ct" HEAD)
KEYSTORE=
KEYSTOREPWD=

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
        --java-home) #
            # set the java vm home directory
            JAVA_HOME=$2
            shift
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
        --ks-pass) #
            # password for the keystore, see apksigner tool for help
            KEYSTOREPWD=${2%/}
            shift
            ;;
        -s|--sdk) #
            # sdk location, required to build an apk
            ANDROID_SDK_ROOT=${2%/}
            shift
            ;;
        --android-platform) # [android-14|...]
            # use a specific android platform target instead of the default one
            ANDROID_PLATFORM=$2
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
    find -name "*.json" -delete -o -name "Makefile" -delete
fi

CURRENT_PREFIX=$APIDIR/host/$BUILD_TYPE/usr
QT_ROOT=$APIDIR/Qt/5.6/android_$BUILD_TYPE
ANDROID_TARGET_ARCH=$BUILD_TYPE
INSTALLDIR=$ROOTDIR/bin/android-$DEBUGRELEASE-installed/$ANDROID_TARGET_ARCH/apex
ANDROID_NDK_ROOT=$APIDIR/android/android-ndk-r9b
PKG_CONFIG_LIBDIR=$CURRENT_PREFIX/lib/pkgconfig
ANT_PATH=$APIDIR/ant/apache-ant-1.9.7/bin/ant
PATH="$APIDIR/build_libs/usr/bin:$PATH" # protoc

export ANDROID_NDK_ROOT JAVA_HOME PKG_CONFIG_LIBDIR ANDROID_SDK_ROOT PATH

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

APEX_SCHEMA_VERSION=$(cat "$ROOTDIR/src/lib/apextools/version.h" | grep '#define APEX_SCHEMA_VERSION' | cut -f 3 -d ' ' | sed 's/"//g')
sed "s#-- %%INSERT_TEST_TEMPLATES%% --#$MANIFEST_TESTS#" "$ROOTDIR/tools/android-template/AndroidManifest.xml.in" \
    | sed "s#-- %%INSERT_VERSION_NAME%% --#$APEX_SCHEMA_VERSION#" \
    | sed "s#-- %%INSERT_VERSION_CODE%% --#$VERSION_CODE#" \
    > "$ROOTDIR/.build/android-$DEBUGRELEASE/template/AndroidManifest.xml"

cp -r $ROOTDIR/src/java/* $ROOTDIR/.build/android-$DEBUGRELEASE/template/src/be/kuleuven/med/exporl/apex/

cp -r $ROOTDIR/tools/android-template/res $ROOTDIR/.build/android-$DEBUGRELEASE/template/

# Build source tree
$QT_ROOT/bin/qmake -spec android-g++ $QMAKEARGS \
                    "PREFIX=$CURRENT_PREFIX" \
                    "PKG_CONFIG_LIBDIR=$CURRENT_PREFIX/lib/pkgconfig" \
                    "QT_CONFIG -= no-pkg-config" \
                    "INCLUDEPATH += $CURRENT_PREFIX/include $QT_ROOT/include" \
                    "LIBS += -L$CURRENT_PREFIX/lib -L$QT_ROOT/lib" \
                    "ANDROID_EXTRA_LIBS += $CURRENT_PREFIX/lib/libprotobuf-lite.so $CURRENT_PREFIX/lib/libxml2.so $CURRENT_PREFIX/lib/libportaudio.so $CURRENT_PREFIX/lib/libsndfile.so $CURRENT_PREFIX/lib/libfftw3f.so $CURRENT_PREFIX/lib/libfftw3.so $ROOTDIR/bin/android-$DEBUGRELEASE/libapexmain.so $CURRENT_PREFIX/lib/libgit2.so" \
                    "ANDROID_PACKAGE_SOURCE_DIR=$ROOTDIR/.build/android-$DEBUGRELEASE/template"
make -j "$JOBS" qmake_all
make -j "$JOBS"
make -j "$JOBS" install "INSTALL_ROOT=$INSTALLDIR"

if [ "$BUILD_APK" = "true" ]; then
    if [ "$ANDROID_SDK_ROOT" = "false" ]; then
        echo "Please specify an sdk location"
        exit 0
    fi

    $QT_ROOT/bin/androiddeployqt --output "$INSTALLDIR" --verbose --input "$ROOTDIR/src/programs/apex/android-libapex.so-deployment-settings.json" --ant "$ANT_PATH" --android-platform "$ANDROID_PLATFORM" $ANDROIDDEPLOYQTARGS

    if [ "$DEBUGRELEASE" = "release" ]; then
        if [ -z "$KEYSTORE" ] || [ -z "$KEYSTOREPWD" ]; then
            echo "Specify --ks and --ks-pass to sign the package"
            exit 1
        fi
        "$ANDROID_SDK_ROOT"/build-tools/24.0.3/zipalign -p 4 \
                           "$INSTALLDIR/bin/QtApp-release-unsigned.apk" \
                           "$INSTALLDIR/bin/QtApp-release-unsigned-aligned.apk"
        "$ANDROID_SDK_ROOT"/build-tools/24.0.3/apksigner sign \
                           --ks "$KEYSTORE" --ks-pass "$KEYSTOREPWD" \
                           --out "$INSTALLDIR/bin/QtApp-$DEBUGRELEASE.apk" \
                           "$INSTALLDIR/bin/QtApp-release-unsigned-aligned.apk"
        "$ANDROID_SDK_ROOT"/build-tools/24.0.3/apksigner verify -v \
                           "$INSTALLDIR/bin/QtApp-$DEBUGRELEASE.apk"
    elif [ "$SIGN" = "true" ] && [ "$DEBUGRELEASE" != "release" ]; then
        echo "Cannot sign debug package"
    fi

    echo "To install do: (it takes about 3 minutes)"
    echo "  adb install -r $INSTALLDIR/bin/QtApp-$DEBUGRELEASE.apk"
fi
