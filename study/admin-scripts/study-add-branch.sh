#!/bin/bash

SCRIPTFILE="$(readlink -f $0)"
cd "$(dirname $SCRIPTFILE)"

STUDY=
HOST="apex-experiments"
EXPERIMENTBRANCH=
YES=
SSHCONFIG=

parsecmd() {
    while (($# > 0)); do
        #CMDSTART
        case "$1" in
            -s|--study) # STUDY
                # study name
                STUDY=$2
                shift
                ;;
            -b|--branch) # EXPERIMENT BRANCH
                # create an experiment branch, the devices (read-only) group has access to this
                EXPERIMENTBRANCH=$2
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
                echo "Add or reconfigure an experimentbranch to study."
                echo
                echo "Usage:"
                echo "   $0 --study studyname --branch experiments"
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

if [ -z "$STUDY" ]; then
    echo "Please specify a study name with --study"
    exit 1
fi

if [ -z "$EXPERIMENTBRANCH" ]; then
    echo "Please specify the name of the experiment branch"
    exit 1
fi

. studies-common.sh

sshcommand "gerrit ls-projects" | grep "^$STUDY\$" -q
if [ $? -ne 0 ]; then
    echo "Study $STUDY doesn't exist"
    exit 1
fi

# group that groups all study-branch-devices groups
GROUPALLDEVICES="$STUDY-devices"
# read-only group is used for the devices with access to the experiment branch
GROUPEXPDEVICES="$STUDY-$EXPERIMENTBRANCH-devices"
# access to every branch
GROUPRW="$STUDY-committers"

sshcommand "gerrit ls-groups" | grep "^$GROUPALLDEVICES\$" -q
if [ $? -ne 0 ]; then
    echo "All devices group doesn't exist"
    exit 1
fi
sshcommand "gerrit ls-groups" | grep "^$GROUPRW\$" -q
if [ $? -ne 0 ]; then
    echo "Commiters group doesn't exist"
    exit 1
fi

sshcommand "gerrit ls-groups" | grep "^$GROUPEXPDEVICES\$" -q
if [ $? -ne 0 ]; then
    sshcommand "gerrit create-group \"$GROUPEXPDEVICES\" --group \"$GROUPRW\" --owner \"$GROUPALLDEVICES\" --description \"$STUDY: $EXPERIMENTBRANCH read access\""
    if [ $? -ne 0 ]; then
        echo "Failed to create experiment branch group"
        exit 1
    fi
fi
GROUPIDEXPDEVICES=$(sshcommand "gerrit gsql -c \"SELECT group_uuid FROM account_groups WHERE name in ('$GROUPEXPDEVICES')\"" | sed -n 's/ //g;3p')

# add the experiment branch devices to all devices
sshcommand "gerrit set-members --include \"$GROUPEXPDEVICES\" \"$GROUPALLDEVICES\""
if [ $? -ne 0 ]; then
    echo "Failed to add study-experimentbranch-devices group to study-devices group"
    exit 1
fi

# Checkout for further changes
tempdir=$(mktemp -d)
pushd "$tempdir"
gitsshcommand clone "ssh://$HOST/$STUDY"
cd "$STUDY"
git reset --hard
gitsshcommand fetch origin meta/config:remotes/origin/meta/config
git checkout -t -b config origin/meta/config

grep "^$GROUPIDEXPDEVICES" groups -q || printf '%s\t%s\n' "$GROUPIDEXPDEVICES" "$GROUPEXPDEVICES" >> groups

git config --file project.config "access.refs/heads/$EXPERIMENTBRANCH.read" "group $GROUPEXPDEVICES"
git config --file project.config "access.refs/heads/$EXPERIMENTBRANCH.push" "group $GROUPRW"

git diff --quiet HEAD
if [ $? -ne 0 ]; then
    if [ "$YES" != "true" ]; then
        GIT_PAGER=cat git diff HEAD
        read -p "Commit access rule changes? " -n 1 -r
        echo
    fi
    if [ "$YES" = "true" ] || [[ $REPLY =~ ^[Yy]$ ]]; then
        echo 'pushing config'
        git commit -a -m "Added experimentbranch $EXPERIMENTBRANCH"
        gitsshcommand push origin config:meta/config
    fi
fi
popd
rm -rf "$tempdir"
