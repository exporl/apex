#!/bin/sh -e

cd `dirname $0`

[ -f ../.git/hooks/commit-msg ] || ln -s $PWD/commit-msg-hook ../.git/hooks/commit-msg

[ -f ../.git/hooks/pre-commit ] || ln -s $PWD/linux-check.sh ../.git/hooks/pre-commit

git submodule foreach git config --replace-all remote.origin.fetch "+refs/changes/*:refs/remotes/origin/changes/*" "changes"
git config --replace-all remote.origin.fetch "+refs/changes/*:refs/remotes/origin/changes/*" "changes"
