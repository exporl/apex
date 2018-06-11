#!/bin/bash

SCRIPTFILE="$(readlink -f $0)"
cd "$(dirname $SCRIPTFILE)"

HOST="apex-experiments"
SSHCONFIG=
EMAIL=

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
            -e|--email) # EMAIL
                # preferred email of account to check
                EMAIL=$2
                shift
                ;;
            --full-name) # FULL NAME
                # full name of account to check
                FULLNAME=$2
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

if [ -z "$FULLNAME" ] && [ -z "$EMAIL" ]; then
    echo "No email or fullname specified."
    exit 1
fi

if [ ! -z "$FULLNAME" ] && [[ ! $(sshcommand "gerrit gsql -c \"SELECT * FROM accounts WHERE full_name LIKE \'$FULLNAME\'\"" | sed -n 's/ //g;3p') =~ "0rows;" ]]; then
    echo true
fi

if [ ! -z "$EMAIL" ] && [[ ! $(sshcommand "gerrit gsql -c \"SELECT * FROM accounts WHERE preferred_email LIKE \'$EMAIL\'\"" | sed -n 's/ //g;3p') =~ "0rows;" ]]; then
    echo true
fi

echo false
