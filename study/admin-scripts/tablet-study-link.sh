#!/bin/bash -e

SCRIPTFILE="$(readlink -f $0)"
cd "$(dirname $SCRIPTFILE)"

TABLETSCRIPTSDIR=~/audio-tablet

. $TABLETSCRIPTSDIR/tools/tablet-common-installer.sh

BOOTLOADER=onlyapk
STUDY=
EXPERIMENTSBRANCH=experiments

parsecmd() {
    while (($# > 0)); do
        #CMDSTART
        case "$1" in
        --study) # [spin-flowrunner]
            # name of the study
            STUDY=$2
            shift
            ;;
        --experiments-branch) # [experiments]
            # experiments branch
            EXPERIMENTSBRANCH=$2
            shift
            ;;
        -h|--help) #
            # this help
            echo "Usage: $0 [OPTION]..."
            echo "Create a new fdroid repo."
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
installersetup
resetadb
selectdevice

GERRIT_DEVICE_ID=${DEVICE}_${DEVICE}
$(dirname $SCRIPTFILE)/connectdevicetostudy.sh --yes --device $GERRIT_DEVICE_ID --study $STUDY --branch $EXPERIMENTSBRANCH

SPEC=$(cat <<EOF
'{"name": "$STUDY", "experimentsUrl": "ssh://${GERRIT_DEVICE_ID}@exporl-ssh.med.kuleuven.be:8444/${STUDY}", "experimentsBranch": "$EXPERIMENTSBRANCH"}'
EOF
)

leavelockscreen
adbsh "am start -W -n be.kuleuven.med.exporl.apex/.ApexActivity -a be.kuleuven.med.exporl.apex.LINKSTUDY --es be.kuleuven.med.exporl.apex.LINKSTUDY $SPEC"
