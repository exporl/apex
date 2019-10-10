#!/bin/bash -e
cd "$(dirname $(readlink -f ${BASH_SOURCE[0]}))/.."

git submodule update --force --init --remote common
common/tools/jenkins-linux-git-setup.sh

npm i

exec common/tools/jenkins-linux-check.sh "$@"
