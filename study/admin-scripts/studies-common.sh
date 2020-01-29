#!/bin/bash -e

function sshcommand {
    if [ -z "$SSHCONFIG" ]; then
        ssh "$HOST" "$1"
    else
        ssh -F "$SSHCONFIG" "$HOST" "$1"
    fi
}

function gitsshcommand {
    if [ ! -z $SSHCONFIG ]; then
        GIT_SSH_COMMAND="ssh -F $SSHCONFIG" git "$@"
    else
        git "$@"
    fi
}
