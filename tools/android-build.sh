#!/bin/bash -e

ROOTDIR=$(pwd)
BUILD_ANDROID=$ROOTDIR/.build/android
CLEAN=false
BUILD_RELEASE=false
BUILD_APK=false

parsecmd() {
    while [ $# -gt 0 ]; do
        #CMDSTART
        case "$1" in
        -e|--extra) #
            # the directory containing all the tools needed to compile. Default is .build/android
            BUILD_ANDROID=$2
            shift
            ;;
        -c|--clean) #
            # executes the command rm -rf .build/android-debug/ android/ bin/android-debug/ && find -name "Makefile" | xargs rm
            CLEAN=true
            ;;
        --build-apk) #
            # attempts to build an apk
            BUILD_APK=true
            ;;
        -r|--release) #
            # build an unsigned release package, expects --build-apk to be true
            BUILD_RELEASE=true
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

if [ $CLEAN = true ]; then
    rm -rf .build/android-debug/ android/ bin/android-debug/ && find -name "Makefile" | xargs rm
fi

EXTRA=$BUILD_ANDROID/extra/usr
ANDROID_NDK_ROOT=$BUILD_ANDROID/android/android-ndk-r9b
JAVA_HOME=$(update-alternatives --list java | grep jdk-i386 | sed "s/\/$(update-alternatives --list java  | grep jdk-i386 | rev | cut -d '/' -f 1,2,3 | rev | sed 's/\//\\\//g')//")
PKG_CONFIG_LIBDIR=$EXTRA/lib/pkgconfig
ANT_PATH=$BUILD_ANDROID/ant/apache-ant-1.9.7/bin/ant
QT_ROOT=$BUILD_ANDROID/Qt/5.6/android_armv7

export ANDROID_NDK_ROOT JAVA_HOME PKG_CONFIG_LIBDIR

$QT_ROOT/bin/qmake -spec android-g++ \
    "PREFIX=$EXTRA" \
    "PKG_CONFIG_LIBDIR=$PKG_CONFIG_LIBDIR" \
    "QT_CONFIG -= no-pkg-config" \
    "INCLUDEPATH += $EXTRA/include $QT_ROOT/include" \
    "LIBS += -L$EXTRA/lib -L$QT_ROOT/lib" \
    "ANDROID_EXTRA_LIBS += $EXTRA/lib/libprotobuf-lite.so $EXTRA/lib/libxerces-c.so $EXTRA/lib/libxerces-c-3.1.so $EXTRA/lib/libxml2.so $EXTRA/lib/libportaudio.so $EXTRA/lib/libjack.so $EXTRA/lib/libasound.so $EXTRA/lib/libjackshm.so $QT_ROOT/lib/libQt5Xml.so $QT_ROOT/lib/libQt5Network.so $QT_ROOT/lib/libQt5Svg.so $QT_ROOT/lib/libQt5Widgets.so $QT_ROOT/lib/libQt5Quick.so $QT_ROOT/lib/libQt5Qml.so $QT_ROOT/lib/libQt5Script.so $QT_ROOT/lib/libQt5ScriptTools.so $QT_ROOT/lib/libQt5PrintSupport.so $QT_ROOT/lib/libQt5XmlPatterns.so"

make -j4
make install INSTALL_ROOT=$ROOTDIR/android

if [ $BUILD_APK = true ]; then
    # Add --install to install directly on an android device
    $QT_ROOT/bin/androiddeployqt --output $ROOTDIR/android --verbose --input $ROOTDIR/src/programs/apex/android-libapex.so-deployment-settings.json --ant $ANT_PATH --android-platform android-14
    if [ $BUILD_RELEASE = true ]; then
        $QT_ROOT/bin/androiddeployqt --output $ROOTDIR/android --verbose --input $ROOTDIR/src/programs/apex/android-libapex.so-deployment-settings.json --ant $ANT_PATH --release --android-platform android-14
    fi

    echo -e "To install do: (it takes about 3 minutes)\n" \
        "adb install $ROOTDIR/android/bin/QtApp-debug.apk"
fi
