#!/bin/bash -e
cd "$(dirname $(readlink -f ${BASH_SOURCE[0]}))/.."

git submodule update --force --init --remote common
common/tools/jenkins-linux-git-setup.sh

pushd documentation >/dev/null
if ! git diff --exit-code HEAD origin/master >/dev/null; then
    echo "Please add latest documentation to commit"
    popd >/dev/null
    exit 1
else
    popd >/dev/null
fi

npm i

exec common/tools/jenkins-linux-check.sh "$@" --exclude "documentation"
