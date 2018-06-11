Building APEX for Android {#Building}
=====================================

The libraries and tools necessary for APEX are built in the
`tools/android-prepare-api.sh` and `tools/android-build.sh` scripts. These
scripts were made for Linux, while it's possible to build on Windows no scripts
or documentation are provided.

Requirements
------------

Other than the requirements found in compiling_instructions you will need the
following:

##### Qt 5.6 for Android

Because Qt 5.7 for Android requires C++11 support, and the ndk used does not
support C++11, Qt 5.6 is required. Ndk r9b is used, newer versions can't build
some of the libraries required.

##### Java 32bit

Qt for Android needs a 32bit java jdk. The script retrieves the location from
updata-java-alternatives. If no i386 installation is found, no apk can be built
and installed, but APEX will build.

##### Android OpenSSL

We compile OpenSSL as a static archive for Android. The SO (shared object) will
conflict with the system's SO (which is not part of the public api) because of
similar symbols. The symbols from the system SO will already be loaded, and APEX
will pick those over the symbols in our SO.

It would work if the SO's had versioned symbols, but there's no way to guarantee
that the system's SO would have these. It could also work if we manage to build
a SO with an ABI identical to the system's SO (for each device), but that's an
impossible task.

By shipping it as a static archive, OpenSSL is simply included within APEX, and
we avoid any calls to `dlopen`.

##### Build tools

Autotools, CMake, and AutoGen are needed for the `tools/android-prepare-api.sh` script.

##### Ubuntu version

No specific Ubuntu version is needed. The scripts should work if all the
dependencies compile with the build tools available on your platform, any
platform with recent versions of Autotools and CMake should do. The scripts were
tested with ubuntu 14, 16, and 18 LTS.

Dependencies and APEX binaries
------------------------------

To crosscompile all the dependencies the `tools/android-prepare-api.sh` script
is provided.

Once all the dependencies are compiled run the `tools/android-build.sh`
script. Run it with the `--help` parameter for all options. Be sure to
provide the `--build-apk` parameter to build an actual apk.

Installing
==========

Release version
---------------

The release version can be installed with F-Droid. Install F-Droid and add
https://exporl.med.kuleuven.be/apex-fdroid/repo as a repository. Simply install
from there.

For information on how to register the device with the **apex-study** interface
you can consult the [documentation on studies](../../studies/index.md).

Debug version
-------------

The debug version which you build yourself can be installed with the Android
debugging bridge (adb).

Android build server
====================

To build the release version on a build server, first follow the [building
steps](#Building) above. `tools/android-prepare-api.sh` can easily take 15
minutes, so make sure the it doesn't have to be called each build.

Building release version
------------------------

To build a release version you'll need to supply the following parameters to
`tools/android-build.sh`:

1. `-r`: for release.

2. `--ks`: path to the keystore containing the key which the apk will be signed
   with. This key is stored in a JKS keystore.

3. `--ks-pass`: this is the keystore password. The safest way to specify the
   password is in a file. The path should be prefixed with "file:".

APKs need to be signed so the user can verify that any next version is issued by
us. The public key certificate included with the APK is self-signed. The JKS
keystore and password should be kept in a safe location. Only the Android build
server should have access to these.

F-Droid
-------

To publish the apk to the fdroid repo, simply copy it to the repository folder
and call the deploy script, see the [release
guide](apex-release-guide.md#releasing-binaries-android) for more information.
