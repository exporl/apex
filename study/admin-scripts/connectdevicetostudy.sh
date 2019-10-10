#!/bin/bash

SCRIPTFILE="$(readlink -f $0)"
cd "$(dirname $SCRIPTFILE)"

HOST="apex-experiments"
STUDY=
DEVICE=
YES=
SSHCONFIG=
ALLOWRESULTS=
EXPERIMENTBRANCHES=()

parsecmd() {
    while (($# > 0)); do
        #CMDSTART
        case "$1" in
            -s|--study) # STUDY
                # study name
                STUDY=$2
                shift
                ;;
            -b|--branch) # BRANCH
                # experiment branch to give device access to, may be specified multiple times
                EXPERIMENTBRANCHES+=("$2")
                shift
                ;;
            -d|--device) # DEVICEID
                # device id
                DEVICE=$2
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
                echo "Link a device to a study"
                echo
                echo "Usage: "
                echo "connectdevicetostudy.sh --study studyname --device deviceid"
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

if [ -z "$STUDY" ]; then
    echo "Please specify study to add the device to"
    exit 1
fi

if [ -z "$DEVICE" ]; then
    echo "Please specify the device name"
    exit 1
fi

if [ -z EXPERIMENTBRANCHES ]; then
    echo "Please specify at least one experiment branch"
    exit 1
fi

. studies-common.sh

echo
echo "Device: $DEVICE"
echo "Adding to study: $STUDY"

if [ "$YES" != "true" ]; then
    read -p "Looks good [y/N]? " -n 1 -r
    echo
    if [[ ! $REPLY =~ ^[Yy]$ ]]; then
        exit 1
    fi
fi

DEVICEGROUPID=$(sshcommand "gerrit gsql -c \"SELECT group_uuid FROM account_groups WHERE name in ('device-$DEVICE')\"" | sed -n 's/ //g;3p')
if [[ "$DEVICEGROUPID" =~ "0rows" ]]; then
    echo "Can't find the device-$DEVICE group, does the device exist?"
    exit 1
fi

sshcommand "gerrit ls-projects" | grep "^$STUDY\$" -q
if [ $? -ne 0 ]; then
    echo "Study $STUDY doesn't exist"
    exit 1
fi

for BRANCH in "${EXPERIMENTBRANCHES[@]}"; do
    echo "Including device-$DEVICE group in $STUDY-$BRANCH-devices"
    sshcommand "gerrit set-members --include device-$DEVICE $STUDY-$BRANCH-devices"
    if [ $? -ne 0 ]; then
        echo "Couldn't include device-$DEVICE group in group $STUDY-$BRANCH-devices"
        exit 1
    fi
done

tempdir=$(mktemp -d)
pushd "$tempdir"
gitsshcommand clone "ssh://$HOST/$STUDY"
cd "$STUDY"
git reset --hard
gitsshcommand fetch origin meta/config:remotes/origin/meta/config
git checkout -t -b config origin/meta/config

grep "^$DEVICEGROUPID" groups -q || printf '%s\t%s\n' "$DEVICEGROUPID" "device-$DEVICE" >> groups

STUDYTYPE=$(git config --file project.config --get "apex.studytype")
if [ "$STUDYTYPE" = "private" ]; then
    git config --file project.config "access.refs/heads/results-$DEVICE.read" "group device-$DEVICE"
    git config --file project.config "access.refs/heads/results-$DEVICE.create" "group device-$DEVICE"
    git config --file project.config "access.refs/heads/results-$DEVICE.push" "group device-$DEVICE"
fi

git diff --quiet HEAD
if [ $? -ne 0 ]; then
    GIT_PAGER=cat git diff HEAD
    if [ "$YES" != "true" ]; then
        read -p "Commit access rule changes? " -n 1 -r
        echo
    fi
    if [ "$YES" = "true" ] || [[ $REPLY =~ ^[Yy]$ ]]; then
        git commit -a -m "Added $DEVICE to study"
        gitsshcommand push origin config:meta/config
    fi
fi
popd
rm -rf "$tempdir"

if [ "$STUDYTYPE" = "private" ]; then
    if [ "$YES" != "true" ]; then
        read -p "Create results-$DEVICE branch? " -n 1 -r
        echo
    fi
    if [ "$YES" = "true" ] || [[ $REPLY =~ ^[Yy]$ ]]; then
        sshcommand "gerrit create-branch $STUDY results-$DEVICE results-base"
    fi
fi
