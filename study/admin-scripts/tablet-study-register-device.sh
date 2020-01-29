#!/bin/bash -e

SCRIPTFILE="$(readlink -f $0)"
cd "$(dirname $SCRIPTFILE)"

TABLETSCRIPTSDIR=~/audio-tablet

. $TABLETSCRIPTSDIR/tools/tablet-common-installer.sh

BOOTLOADER=onlyapk

installersetup
resetadb
selectdevice

TEMP_DIR=$(mktemp -d) && pushd ${TEMP_DIR}

adbsh "am start -W be.kuleuven.med.exporl.apex/.ApexActivity" # start apex app to ensure a ssh-keypair
sleep 5 # ssh-kepair is generated after some seconds
adbsh "am force-stop be.kuleuven.med.exporl.apex"
adbsu "cp /data/data/be.kuleuven.med.exporl.apex/files/studymanager/id_rsa.pub /sdcard/id_rsa.pub"
adbpull /sdcard/id_rsa.pub ./id_rsa.pub
adbsh "rm /sdcard/id_rsa.pub"

if [ ! -f id_rsa.pub ]; then
    echo "Public key not found on device"
    exit 1
fi

$(dirname $SCRIPTFILE)/add-device.sh --yes --keyfile $TEMP_DIR/id_rsa.pub

rm -rf ${TEMP_DIR} && popd
