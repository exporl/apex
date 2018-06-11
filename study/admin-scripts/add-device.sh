#!/bin/bash

SCRIPTFILE="$(readlink -f $0)"
cd "$(dirname $SCRIPTFILE)"

HOST="apex-experiments"
DEVICE=
KEYFILE=
KEYSTRING=
YES=
SSHCONFIG=

parsecmd() {
    while (($# > 0)); do
        #CMDSTART
        case "$1" in
            -d|--device) # DEVICE
                # device ID
                DEVICE=$2
                shift
                ;;
            -k|--keyfile) # PATH TO PUBKEY
                # path to device's public keyfile
                KEYFILE=$2
                shift
                ;;
            --key)
                KEYSTRING=$2
                shift
                ;;
            --host) # HOST
                # contact a different server instead of apex-experiments
                HOST=$2
                shift
                ;;
            --ssh-config) # FILEPATH
                # path to ssh-config file
                SSHCONFIG=$2
                shift
                ;;
            -y|--yes) # assume yes
                # don't ask confirmations
                YES=true
                ;;
            -h|--help) #
                # this help
                echo "Usage: $0 [OPTION]..."
                echo
                echo "Create a device if it doesn't exist, modify it otherwise"
                echo
                echo "Usage: "
                echo "add-device.sh --study studyname --device deviceid --keyfile device's key in a file"
                echo
                echo "Please define the host gerrit in your .ssh/config like this (or use --host):"
                echo "   Host apex-experiments"
                echo "   HostName exporl-ssh.med.kuleuven.be"
                echo "   User u012345"
                echo "   Port 8444"
                echo
                echo "Command line arguments:"
                echo
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

if [ -z "$HOST" ]; then
    echo "Please specify host to connect to"
    exit 1
fi

if [ -z "$KEYSTRING" ] && [ -f "$KEYFILE" ]; then
    echo "Keystring empty but keyfile specified, getting keystring from keyfile"
    KEYSTRING=$(cat "$KEYFILE")
fi

if [ -z "$DEVICE" ] && [ ! -z "$KEYSTRING" ]; then
    echo "Device empty but keystring specified, getting device from keystring"
    DEVICE=$(echo "$KEYSTRING" | cut -d' ' -f 3 | tr '@' '_')
fi

if [ -z "$DEVICE" ]; then
    echo "Please specify the device name"
    exit 1
fi

. studies-common.sh

# create if device doesn't exist
EXISTS=false
if [ "$DEVICE" = $(sshcommand "gerrit gsql -c \"SELECT FULL_NAME FROM accounts WHERE FULL_NAME in ('$DEVICE')\"" | sed -n 's/ //g;3p') ]; then
    EXISTS=true
fi

echo
echo "Device: $DEVICE"
if [ "$EXISTS" = "false" ]; then
    echo "Device doesn't exist, will create account($DEVICE) and group(device-$DEVICE)"
    echo "Email: $(echo "$KEYSTRING" | cut -d' ' -f 3)"
    echo "Key: $KEYSTRING"
else
    echo "Device exists, will add ssh key and email to account"
    echo "Email: $(echo "$KEYSTRING" | cut -d' ' -f 3)"
    echo "Key: $KEYSTRING"
fi
if [ "$YES" != "true" ]; then
    read -p "Looks good [y/N]? " -n 1 -r
    echo
    if [[ ! $REPLY =~ ^[Yy]$ ]]; then
        exit 1
    fi
fi

if [ -z "$KEYSTRING" ]; then
    echo "Please specify key"
    exit 1
fi
EMAIL=$(echo "$KEYSTRING" | cut -d' ' -f 3)
if [ -z "$EMAIL" ]; then
    echo "Error getting email from $KEYSTRING, this should be the comment."
    exit 1
fi

if [ "$EXISTS" = "false" ]; then
    sshcommand "gerrit create-group \"device-$DEVICE\" --description \"Personal group for $DEVICE\""
    if [ $? -ne 0 ]; then
        echo "Unable to create group device-$DEVICE"
    fi
    echo "$KEYSTRING" | sshcommand "gerrit create-account \"$DEVICE\" --group \"device-$DEVICE\" --full-name \"$DEVICE\" --email \"$EMAIL\" --ssh-key -"
    if [ $? -ne 0 ]; then
        echo "Unable to create account $DEVICE"
    fi
else
    # if device exists but group doesn't, create group
    DEVICEGROUPID=$(sshcommand "gerrit gsql -c \"SELECT group_uuid FROM account_groups WHERE name in ('device-$DEVICE')\"" | sed -n 's/ //g;3p')
    if [[ "$DEVICEGROUPID" =~ "0rows" ]]; then
        sshcommand gerrit "create-group \"device-$DEVICE\" --description \"Personal group for $DEVICE\""
    fi
    if [ $? -ne 0 ]; then
        echo "Unable to create group device-$DEVICE"
        exit 1
    fi
    # if device exists and group does, add device just in case
    sshcommand "gerrit set-members --add \"$DEVICE\" \"device-$DEVICE\""
    if [ $? -ne 0 ]; then
        echo "Unable to add $DEVICE to group device-$DEVICE"
        exit 1
    fi
    # add ssh-key to device, as it might be new
    echo "$KEYSTRING" | sshcommand "gerrit set-account \"$DEVICE\" --add-ssh-key -"
fi

DEVICEGROUPID=$(sshcommand "gerrit gsql -c \"SELECT group_uuid FROM account_groups WHERE name in ('device-$DEVICE')\"" | sed -n 's/ //g;3p')
if [[ "$DEVICEGROUPID" =~ "0rows" ]]; then
    echo "Can't find the device-$DEVICE group, did something go wrong during device creation?"
    exit 1
fi
