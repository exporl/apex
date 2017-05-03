copy tools\commit-msg-hook .git\hooks\commit-msg

git submodule foreach git config --replace-all remote.origin.fetch "+refs/changes/*:refs/remotes/origin/changes/*" "changes"
git config --replace-all remote.origin.fetch "+refs/changes/*:refs/remotes/origin/changes/*" "changes"
