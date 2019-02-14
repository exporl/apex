#!/bin/bash

SCRIPTFILE="$(readlink -f $0)"
cd "$(dirname $SCRIPTFILE)"

STUDY=
DESCRIPTION=
HOST="apex-experiments"
EXPERIMENTBRANCH=
YES=
COMMITTERS=
SSHCONFIG=
STUDYTYPE=

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
            -d|--description) # DESCRIPTION
                # project description
                DESCRIPTION=$2
                shift
                ;;
            --committers) # USERNAME
                # Initial member of committers group
                COMMITTERS="--member $2"
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
            --public) # public study, no results allowed
                if [ ! -z "$STUDYTYPE" ]; then
                    echo "Studytype already specified"
                    exit 1
                fi
                STUDYTYPE="public"
                ;;
            --private) # private study, no results allowed
                if [ ! -z "$STUDYTYPE" ]; then
                    echo "Studytype already specified"
                    exit 1
                fi
                STUDYTYPE="private"
                ;;
            -h|--help) #
                # this help
                echo "Usage: $0 [OPTION]..."
                echo
                echo "Create a new apex study."
                echo
                echo "To create a study:"
                echo "   $0 --study studyname --description 'Study description' --branch experiments --private"
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

if [ -z "$DESCRIPTION" ]; then
    echo "Please specify a description with --description"
    exit 1
fi

if [ -z "$EXPERIMENTBRANCH" ]; then
    echo "Please specify the name of the experiment branch"
    exit 1
fi

if [ -z "$STUDYTYPE" ]; then
    echo "Please specify the study type (--public or --private)"
    exit 1
fi

. studies-common.sh

# group that groups all study-branch-devices groups
GROUPALLDEVICES="$STUDY-devices"
# read-only group is used for the devices with access to the experiment branch
GROUPEXPDEVICES="$STUDY-$EXPERIMENTBRANCH-devices"
# access to every branch
GROUPRW="$STUDY-committers"

echo
echo "Study: $STUDY"
echo "Description: $DESCRIPTION"
echo "Devices (experiment branches only) group: $GROUPEXPDEVICES"
echo "Committers group: $GROUPRW"
echo "Experiment branch: $EXPERIMENTBRANCH"
if [ "$YES" != "true" ]; then
    read -p "Looks good [y/N]? " -n 1 -r
    echo
    if [[ ! $REPLY =~ ^[Yy]$ ]]; then
        exit 1
    fi
fi

# Create groups
sshcommand "gerrit ls-groups" | grep "^$GROUPRW\$" -q
if [ $? -ne 0 ]; then
    sshcommand "gerrit create-group \"$GROUPRW\" $COMMITTERS --description \"$DESCRIPTION: write and result access\""
    if [ $? -ne 0 ]; then
        echo "Failed to create RW group"
        exit 1
    fi
fi
GROUPIDRW=$(sshcommand "gerrit gsql -c \"SELECT group_uuid FROM account_groups WHERE name in ('$GROUPRW')\"" | sed -n 's/ //g;3p')

sshcommand "gerrit ls-groups" | grep "^$GROUPALLDEVICES\$" -q
if [ $? -ne 0 ]; then
    sshcommand "gerrit create-group \"$GROUPALLDEVICES\" --group \"$GROUPRW\" --owner \"$GROUPRW\" --description \"$DESCRIPTION: group for all groups with experiment read access\""
    if [ $? -ne 0 ]; then
        echo "Failed to create all devices group"
        exit 1
    fi
fi
GROUPIDALLDEVICES=$(sshcommand "gerrit gsql -c \"SELECT group_uuid FROM account_groups WHERE name in ('$GROUPALLDEVICES')\"" | sed -n 's/ //g;3p')

sshcommand "gerrit ls-groups" | grep "^$GROUPEXPDEVICES\$" -q
if [ $? -ne 0 ]; then
    sshcommand "gerrit create-group \"$GROUPEXPDEVICES\" --group \"$GROUPRW\" --owner \"$GROUPALLDEVICES\" --description \"$DESCRIPTION: $EXPERIMENTBRANCH read access\""
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

# Create project
sshcommand "gerrit ls-projects" | grep "^$STUDY\$" -q
if [ $? -ne 0 ]; then
    sshcommand "gerrit create-project \"$STUDY\" --description \"$DESCRIPTION\" --branch \"$EXPERIMENTBRANCH\""
    if [ $? -ne 0 ]; then
        echo "Failed to create project"
        exit 1
    fi
fi

# Checkout for further changes
tempdir=$(mktemp -d)
pushd "$tempdir"
gitsshcommand clone "ssh://$HOST/$STUDY"
cd "$STUDY"
git reset --hard
gitsshcommand fetch origin meta/config:remotes/origin/meta/config
git checkout -t -b config origin/meta/config

# Set access rights
touch groups
grep "^$GROUPIDRW" groups -q || printf '%s\t%s\n' "$GROUPIDRW" "$GROUPRW" >> groups
grep "^$GROUPIDEXPDEVICES" groups -q || printf '%s\t%s\n' "$GROUPIDEXPDEVICES" "$GROUPEXPDEVICES" >> groups
grep "^$GROUPIDALLDEVICES" groups -q || printf '%s\t%s\n' "$GROUPIDALLDEVICES" "$GROUPALLDEVICES" >> groups

# Don't require change id so devices can push straight to their branch
git config --file project.config "receive.requireChangeId" "false"

# RO
git config --file project.config "access.refs/heads/$EXPERIMENTBRANCH.read" "group $GROUPEXPDEVICES"

# RW
git config --file project.config "access.refs/heads/*.create" "group $GROUPRW"
git config --file project.config "access.refs/heads/*.submit" "group $GROUPRW"
git config --file project.config "access.refs/heads/*.label-Verified" "-1..+1 group $GROUPRW"
git config --file project.config "access.refs/heads/*.label-Code-Review" "-2..+2 group $GROUPRW" "$GROUPRW\$"
git config --file project.config "access.refs/changes/*.push" "group $GROUPRW"
git config --file project.config "access.refs/for/*.push" "group $GROUPRW"
# needed for gitweb?
git config --file project.config "access.refs/*.read" "group $GROUPRW"
# needed for gitweb
git config --file project.config "access.refs/meta/config.read" "group $GROUPRW"

# study type
git config --file project.config "apex.studytype" "$STUDYTYPE"

git add groups
git diff --quiet HEAD
if [ $? -ne 0 ]; then
    GIT_PAGER=cat git diff HEAD
    if [ "$YES" != "true" ]; then
        read -p "Commit access rule changes? " -n 1 -r
        echo
    fi
    if [ "$YES" = "true" ] || [[ $REPLY =~ ^[Yy]$ ]]; then
        git commit -a -m "Script access rule change"
        gitsshcommand push origin config:meta/config
    fi
fi

popd
rm -rf tempdir
