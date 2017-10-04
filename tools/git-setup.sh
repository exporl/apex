#!/bin/bash -e

cd "$(dirname $(readlink -f ${BASH_SOURCE[0]}))/.."

[ -f .git/hooks/commit-msg ] || ln -s "$PWD"/tools/commit-msg-hook .git/hooks/commit-msg

[ -f .git/hooks/pre-commit ] || ln -s "$PWD"/tools/linux-check.sh .git/hooks/pre-commit

[ -f .git/hooks/pre-push ] || ln -s "$PWD"/tools/pre-push-hook .git/hooks/pre-push

git submodule foreach '[ -f "$(git rev-parse --git-dir)/hooks/commit-msg" ] || ln -s "$PWD"/../tools/commit-msg-hook "$(git rev-parse --git-dir)/hooks/commit-msg"'
git submodule foreach '[ -f "$(git rev-parse --git-dir)/hooks/pre-push" ] || ln -s "$PWD"/../tools/pre-push-hook "$(git rev-parse --git-dir)/hooks/pre-push"'

git submodule foreach git config --replace-all remote.origin.fetch "+refs/changes/*:refs/remotes/origin/changes/*" "changes"
git config --replace-all remote.origin.fetch "+refs/changes/*:refs/remotes/origin/changes/*" "changes"
