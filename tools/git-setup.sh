#!/bin/sh -e

cd `dirname $0`

[ -f ../.git/hooks/commit-msg ] || ln -s $PWD/commit-msg-hook ../.git/hooks/commit-msg

[ -f ../.git/hooks/pre-commit ] || ln -s $PWD/jenkins-linux-check.sh ../.git/hooks/pre-commit
