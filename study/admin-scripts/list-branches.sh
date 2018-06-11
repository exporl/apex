#!/bin/bash

SCRIPTFILE="$(readlink -f $0)"
cd "$(dirname $SCRIPTFILE)"

HOST="apex-experiments"
SSHCONFIG=

parsecmd() {
    while (($# > 0)); do
        #CMDSTART
        case "$1" in
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
            -s|--study) # STUDY
                # study to list the experiment branches of
                STUDY=$2
                shift
                ;;
            -h|--help) #
                # this help
                echo "Usage: $0 [OPTION]..."
                echo
                echo "Lists experimentbranches studies."
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

. studies-common.sh

if [ -z "$STUDY" ]; then
    echo "Please specify a study name with --study"
    exit 1
fi

for GROUP in $(sshcommand "gerrit ls-groups -p \"$STUDY\""); do
    if [[ "$GROUP" =~ ^"$STUDY"-.+-devices$ ]]; then
        echo "${GROUP%-devices}" | sed "s/$STUDY-//"
    fi
done
