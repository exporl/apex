#!/bin/bash -e

cd "$(dirname $(readlink -f ${BASH_SOURCE[0]}))/.."

ROOTDIR=$(pwd)
APK=bin/android-debug-installed/armv7/apex/build/outputs/apk/debug/apex-debug.apk
ONJENKINS=false

parsecmd() {
    while [ $# -gt 0 ]; do
        #CMDSTART
        case "$1" in
        -a|--apk) # [apex.apk]
            # the apk file to test
            APK=$2
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

enablescreenandunlock() {
    # Enable screen on Nexus 7
    while adb shell dumpsys power | grep mScreenOn=false; do
        adb shell input keyevent 26
        sleep 1
    done
    # Home button Samsung
    adb shell "input keyevent 3"
    # Nexus 7 (2nd) vertical
    adb shell "input swipe 600 1350 1200 1350"
    # Nexus 7 (2nd) horizontal
    adb shell "input swipe 1350 600 1350 1200"
    # Samsung vertical
    adb shell "input swipe 400 900 800 900"
    # Samsung horizontal
    adb shell "input swipe 900 400 900 800"
}

parsecmd "$@"

echo "Package: $APK"

(
    flock 9

    echo "Starting adb server"
    adb kill-server
    export ADB_VENDOR_KEYS="$ROOTDIR/tools/adbkey"
    adb start-server

    echo "Installing apex and preparing test bed"
    adb shell pm uninstall be.kuleuven.med.exporl.apex
    adb install "$APK"
    adb shell mkdir -p /sdcard/temp/tests
    adb logcat -c
    adb shell "settings put system screen_off_timeout 120000"

    adb shell rm -rf /sdcard/temp/tests/*

    for i in $ROOTDIR/*/tests/*/*.pro; do
        TARGET=$(grep TARGET "$i" | sed 's/.*=//;s/ //g')
        if [ -z "$TARGET" ]; then
            TARGET=$(basename "$i" .pro)
        fi
        rm -f "$ROOTDIR/$TARGET-results.xml" "$ROOTDIR/$TARGET-results.txt"
        if [ ! -f "$ROOTDIR/bin/android-debug/lib${TARGET}.so" ]; then
            echo "test library for $TARGET not found, skipping"
            continue
        fi
        echo "Testing $TARGET (${TARGET^}Activity)"
        adb shell am force-stop be.kuleuven.med.exporl.apex
        sleep 3
        enablescreenandunlock
        ARGUMENTS="\"-o /sdcard/temp/tests/$TARGET-results.xml,xunitxml -o /sdcard/temp/tests/$TARGET-results.txt,txt\""
        adb shell am start -e applicationArguments "$ARGUMENTS" -n be.kuleuven.med.exporl.apex/be.kuleuven.med.exporl.apex.${TARGET^}Activity
        # allow Ctrl-C interrupt by relaying any SIGINT to the process group of timeout, and exiting the script
        trap 'kill -INT -$pid; exit' INT
        timeout 600 bash -c "while [ ! -s $TARGET-results.xml ]; do adb pull /sdcard/temp/tests/$TARGET-results.xml $ROOTDIR || true; sleep 5; done" &
        pid=$!
        wait $pid
        trap - INT
        sleep 1s
    done

    # wait until XML results are complete, then copy them over
    sleep 10s
    for i in $ROOTDIR/*/tests/*/*.pro; do
        TARGET=$(grep TARGET "$i" | sed 's/.*=//;s/ //g')
        adb pull "/sdcard/temp/tests/$TARGET-results.xml" "$ROOTDIR" || true
        adb pull "/sdcard/temp/tests/$TARGET-results.txt" "$ROOTDIR" || true
    done

    #adb shell rm -rf /sdcard/temp/tests
    adb shell am force-stop be.kuleuven.med.exporl.apex

) 9> /tmp/jenkins-android-device.lock
