#!/bin/bash -e

cd "$(dirname $(readlink -f ${BASH_SOURCE[0]}))/.."

ROOTDIR=$(pwd)
TARGET_HOST=arm-linux-androideabi
PACKAGE=be.kuleuven.med.exporl
APP="$PACKAGE".apex
DEVICE_LIBRARIES=
NDK_ROOT=
EXTRA_LIBS=
CLEAN=false
STARTAPP=false
ACTIVITY=ApexActivity

parsecmd() {
    while [ $# -gt 0 ]; do
        #CMDSTART
        case "$1" in
            --ndk) #
                # root directory of standalone ndk.
                NDK_ROOT=${2%/}
                shift
                ;;
            --app) #
                # package with the app (e.g. be.kuleuven.med.exporl.apex).
                PACKAGE=${2%.*}
                APP=${2##*.}
                shift
                ;;
            --activity) #
                # activity to start
                ACTIVITY=$2
                shift
                ;;
            --clean) #
                # clean the workspace
                CLEAN=true
                ;;
            --start-app) #
                # Start the app and gdb server.
                # Otherwise start the gdb server from the app itself.
                STARTAPP=true
                ;;
            --so-paths) #
                # Any extra so paths to be included, colon-separated.
                # Recommended is the qt libraries (and the api libraries.
                EXTRA_LIBS=$2
                shift
                ;;
            --x86) #
                # x86 target host
                TARGET_HOST=i686-linux-android-gdb
                shift
                ;;
            --device-libraries) #
                # the directory containing system-libs, vendor-libs,
                # app_process, linker.
                DEVICE_LIBRARIES=${2%/}
                shift
                ;;
            -h|--help) #
                # this help
                # Usage if starting the app from host:
                # ./tools/android-gdb.sh --start-app --ndk ~/android/android_standalone_armv7 --so-paths "~/libs_apex_android/Qt/5.6/android_armv7/lib/:~/libs_apex_android/host/armv7/usr/lib"
                # Usage if starting the gdb server from the app:
                # ./tools/android-gdb.sh --ndk ~/android/android_standalone_armv7 --so-paths "~/libs_apex_android/Qt/5.6/android_armv7/lib/:~/libs_apex_android/host/armv7/usr/lib"
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
    rm -rf "$ROOTDIR"/.build/android-gdb
fi

mkdir -p "$ROOTDIR"/.build/android-gdb
GDB_DIR="$ROOTDIR"/.build/android-gdb

if [ -z "$DEVICE_LIBRARIES" ]; then
    DEVICE_LIBRARIES="$GDB_DIR"/device-libraries
fi
if [ ! -f "$DEVICE_LIBRARIES"/linker ] || [ ! -f "$DEVICE_LIBRARIES"/app_process ]; then
    mkdir -p "$DEVICE_LIBRARIES"
    mkdir -p "$DEVICE_LIBRARIES"/system-lib
    mkdir -p "$DEVICE_LIBRARIES"/vendor-lib
    adb pull /system/lib "$DEVICE_LIBRARIES"/system-lib
    adb pull /vendor/lib "$DEVICE_LIBRARIES"/vendor-lib
    adb pull /system/bin/app_process "$DEVICE_LIBRARIES"
    adb pull /system/bin/linker "$DEVICE_LIBRARIES"
fi

if [ "$STARTAPP" = "true" ]; then
    GDBPID=$(adb shell ps | grep libgdbserver.so | awk '{ print $2 }')
    [ -z "$GDBPID" ] || adb shell run-as "$APP" kill "$GDBPID"
    adb shell am force-stop "$APP"
    adb forward tcp:5039 tcp:5039
    adb shell am start -n "$APP"/"$APP"."$ACTIVITY"
    APPPID=$(adb shell ps | grep "$APP" | awk '{ print $2 }')
    adb shell run-as "$APP" /data/data/"$APP"/lib/libgdbserver.so :5039 --attach "$APPPID" &
fi

sleep 2 # some time to start the gdbserver
"$NDK_ROOT"/bin/"$TARGET_HOST"-gdb "$DEVICE_LIBRARIES"/app_process \
                               -ex "set sysroot $NDK_ROOT/sysroot" \
                               -ex "set solib-search-path $ROOTDIR/bin/android-debug:$EXTRA_LIBS:$NDK_ROOT/sysroot/lib:$DEVICE_LIBRARIES:$DEVICE_LIBRARIES/vendor-lib:$DEVICE_LIBRARIES/system-lib" \
                               -ex "target remote :5039"
