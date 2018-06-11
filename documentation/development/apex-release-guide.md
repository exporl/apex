Preparing a release version of APEX
===================================

Requirements
------------

The release script and this guide was made for linux. Furthermore Oxygen should
be installed to generate the schema documentation.

To release the binaries you'll also need access to the webserver and the build
server.

Making the changes
------------------

The linux-new-version script in tools makes all the needed changes to the
worktree, it will:

* Spawn an editor to edit the version header.
* Update the schema version of the XML and XSD files found in data/schema,
  data/config, and in examples.
* Call the apex-schema-documentation script, which&mdash;when supplied with
  the `--oxygen` directory&mdash;will make the necessary changes to the
  apex-documentation-schemas worktree.

The changes will still need to be committed. Start in
apex-documentation-schemas, which is a submodule of apex-documentation. Then
commit the updated submodule to apex-documentation. As a final step commit the
documentation submodule together with the changes made in the Apex root project.

Releasing binaries
------------------

Binaries should be released for Windows and Android. Do this after the
changes have been committed and a master build has been completed.

### Windows

Copy the installer (`.msi` file) built by the buildserver to the folder with
similar files. Then add a link on the downloads page to the new installer.

### Android {#releasing-binaries-android}

To release a new Android version to the F-Droid repository, trigger the
apex-android-release project on the buildserver. It's best to do this right
after the master build for the Windows binaries as it will build the latest
commit of the master branch.

When the build is completed the buildserver will trigger the
apex-android-release-deploy project on the buildserver. This will call the
deploy script on the buildserver. See the tools/fdroid folder as an example.

Github code dump
----------------

As APEX is open source, the code is uploaded to github with each release. This
is a simple code dump without git history and without submodules. Note that the
content of the submodules should be committed, just the `.gitmodules` file and
the `.git` folders in each of the submodules should not be committed.

Copy all the files (with exception of the `.git` folder) from your APEX gerrit
clone to an APEX github clone, and commit. It's advised to `git clean -fxd` in
your APEX gerrit clone before copying to make sure no binaries or unnecessary
files get copied. The result should be similar to this:

```shell
git clean -fxd
git submodule foreach git clean -fxd
find -type f -not -path "*/.git/*" \
    -and -not -path "*/.git" \
    -and -not -path "*.gitmodules" \
    -exec \
    cp -f --parents {} /path/to/your/apex/github/clone \;
```
