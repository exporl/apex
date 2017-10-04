#!/bin/bash -e

cd "$(dirname ${BASH_SOURCE[0]})/../.."

git submodule update --force --init --remote
git submodule foreach git config --replace-all remote.origin.fetch "+refs/changes/*:refs/remotes/origin/changes/*" "changes"
git submodule foreach git fetch
git submodule update
