Studies
=======

Studies within APEX are a way of conveniently managing experiments and results.
Using Libgit2, APEX can download experiments from a git repository and
optionally upload results to a branch in that repository.

Public studies only download experiments, while private studies will also
attempt to upload their results to the repository.

Note that this documentation refers to an internal page for managing studies,
and as such is not (completely) applicable to external installations [^1].

Getting started
---------------

### creating a study

Go to `https://exporl.med.kuleuven.be/apex-study/` and press the
`Studies` button. On the studies page press `Add study`. Fill in the
form and press `Submit`.

You can also update a study (add branches and view linked
devices) on the studies page.

### linking to public study

1. In APEX go to `file > link a new study`. If no studies are present you will
   be sent to the `Secure Shell keys` tab.

2. Press the `Share` button and a browser with this page will be opened, with
   the url already filled in.

3. Give the device access to your study. At
   `https://exporl.med.kuleuven.be/apex-study/` go to `Link device to study`.
   Click on a device (or multiple ones), select a study and one or more branches
   and press `Link`.

4. Then go to `New...` on the `Studies` tab.

5. Paste the url to your git repository inside the `Url` field.
   Libgit2 can use http/https[^2] or ssh. 
   In the the `https://exporl.med.kuleuven.be/apex-study/` web interface, this 
   url can be copied from the 'Study' tab. The interface will create a user with the same name as your device, so an example url 
   could be `ssh://3102237ea402b301@exporl-ssh.med.kuleuven.be:8444/name-of-repo`

6. Uncheck `Upload results`. Select the branch containing the experiment files,
   and click `Link study`.

### linking to private study

1. In APEX go to `file > link a new study`. If no studies are present you will
   be sent to the `Secure Shell keys` tab.

2. Press the `Share` button and a browser with this page will be opened, with
   the url already filled in.

3. Give the device access to your study. At
   `https://exporl.med.kuleuven.be/apex-study/` go to `Link device to study`.
   Click on a device (or multiple ones), select a study and one or more branches
   and press `Link`.

4. Then go to `New...` on the `Studies` tab.

5. Paste the url to your git repository here. It is recommended to use the ssh
   protocol.

6. Select the branch containing the experiment files. If no branches show up,
   make sure the machine has access to your repository. Once a branch is
   selected click `Link study`.

### uploading experiments

To upload new experiments all that is needed is to push experiments to a
specific branch of the git repository. For instance if a study was created with
an experiment branch called `experiments-1` the workflow would be as followed:

```shell
# Clone the repository
# you can find the url on
# https://exporl.med.kuleuven.be/apex-experiments/
# under projects.
git clone <url>
cd <repository>
# First create a commit.
git add <experiment files and stimuli>
git commit

# Push to the experiment branch.
git push origin HEAD:refs/heads/experiments-1
```

If there is a flowrunner (.apf file) present in the root directory of the
project, Apex will open that file when starting the study. If no flowrunners are
present in the project root directory, the user will be able to choose out of a
list containing all the apf and apx files present in the project (so including
subfolders) each time the study is started.

If multiple flowrunners are present in the project root directory, *index.apf*
will take precedence. If the latter is not present, one is chosen at random.

### fetching results

Result branches are prefixed with `results-`. `git branch
--list -a origin/results-*` lists all result branches.

```shell
# Clone the repository
# you can find the url on
# https://exporl.med.kuleuven.be/apex-experiments/
# under projects.
git clone <url>
cd <repository>

# Checkout 'all-results' branch to collect the results on.
git checkout all-results

# List all result branches.
BRANCHES=$(git branch -a --list origin/results-*)
# Merge all branches onto all-results.
git merge $BRANCHES
# You can push the merged branches back to origin (optional)
git push origin HEAD:all-results
```

The results should now be collected on the `all-results` branch, and
present in your working directory. To update the branch with new results, do
`git fetch origin` and then repeat the steps in
the above procedure starting from `# Checkout 'all-results' branch`.

Security
--------

For private studies it is recommended to use the ssh protocol. And to set up the
server in such a way that a single device can only access the experiment branch
(read-only) and the results branch belonging to that device. This way a device
cannot read results belonging to other devices.

#### Notes
[^1]: For more technical information see [Setting up Gerrit for use with Apex
    studies](../development/misc/studies-gerrit#Setting up Gerrit for use with
    Apex studies)

[^2]: Note that on Linux Libgit2 provided by the package manager might not be
    able to use the https protocol because of OpenSSL license issues.
