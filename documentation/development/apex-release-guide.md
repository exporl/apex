Preparing a release version of APEX
===================================

The release script and this guide was made for linux. Furthermore Oxygen should
be installed to generate the schema documentation.

To release the binaries you'll also need access to the webserver and jenkins.

## Code changes when releasing

### Minor version change

The `linux-new-version.sh` script makes all the needed changes to the
worktree, it will:

* Spawn an editor to edit the version header.
* Update the schema version of the XSD files in data/schemas.
* Update the XML and APX files found in data/config and examples to use the new schema version.
* Call the apex-schema-documentation script, which&mdash;when supplied with
  the `--oxygen` directory&mdash;will make the necessary changes to the
  apex-documentation-schemas worktree.
* Update the wix build and configuration files to make sure the correct version is embedded in the installer and the installer filename.

```shell
tools/linux-new-version.sh --oxygen path/to/oxygen/directory
```

The changes will still need to be committed. Start in
apex-documentation-schemas, which is a submodule of apex-documentation. Then
commit the updated submodule to apex-documentation. As a final step commit the
documentation submodule together with the changes made in the Apex root project.

Make a tag so it's clear which code is released under the new version number with:
```shell
git tag x.x.x
git push origin x.x.x
git ls-remote --tags origin (to review remote tags)
```

### Major version change

When increasing a major version, some more manual work will be required. The
following will need to be updated:

* GPL header in header and source files.
* Licenses in documentation and the documentation itself.
* The website's index, download and registration page.
* Github project description.
* Jenkins configurations: archiving artifacts, publishing artifacts.


## Publishing artefacts

Binaries should be published for Windows and Android. Do this after the
changes have been committed and a master build has been completed.

### Windows

Copy the installer from the network drive to `/var/www/apex/download`. On the webserver do:
```shell
cp /mnt/x/projects_0013/apex_0003/x.x/yyyy-mm-dd_HH-MM/apex_x.x.x.msi /var/www/apex/download
```

Then add a link on the downloads page (`/var/www/apex/register/downloads.html`) to the new installer.

### Android

To publish a new Android version to the F-Droid repository, trigger the
`apex-publish-to-fdroid` project on jenkins with the parameter `release`. It's best to do this right
after the master build for the Windows binaries as it will publish the latest
apk built by the master build. When successful the apk will be available to devices using fdroid.

### Schema

Copy the xsd files from your local working directory to the webserver so they are available
through http. From your local apex worktree do:

```shell
rsync -r data/schemas/*.xsd username@exporl-ssh.med.kuleuven.be:/var/www/apex/schemas/x.x.x
```

### Github code dump

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
