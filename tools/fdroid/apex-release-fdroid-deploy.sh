#!/bin/bash -e
# Example of F-Droid deploy script to be used on development server.

cd $HOME

if [ -z "$(findmnt | grep /var/lib/jenkins/apex-release-fdroid/repo)" ]; then
    echo 'Release apk repo not mounted, run:'
    echo 'sudo mount --bind /mnt/gbw_shares3/GBW-0226_ExpORL/projects_0013/apex_0003/fdroid-release-repo /var/lib/jenkins/apex-release-fdroid/repo'
    exit 1
fi

if [ -z "$(findmnt | grep /var/lib/jenkins/apex-release-fdroid/archive)" ]; then
    echo 'Release apk archive not mounted, run:'
    echo 'sudo mount --bind /mnt/gbw_shares3/GBW-0226_ExpORL/projects_0013/apex_0003/fdroid-release-archive /var/lib/jenkins/apex-release-fdroid/archive'
    exit 1
fi

pushd apex-release-fdroid
fdroid update "$@"
popd
