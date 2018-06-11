cd %~dp0\..\..

git clean -fdx

git submodule update --force --init --remote
git submodule foreach git config --replace-all remote.origin.fetch "+refs/changes/*:refs/remotes/origin/changes/*" "changes"
git submodule foreach git fetch
git submodule update
