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

##### Qt for Android

Installation automated by the `tools/android-prepare-api.sh` script.

##### Java

##### Android OpenSSL

We compile OpenSSL as a shared archive for Android. Special care needs to be taken to avoid loading the system libraries, which are OpenSSL 1.0. See <https://doc.qt.io/qt-5/android-openssl-support.html> for more information about how to build OpenSSL for android.

##### Build tools

Autotools, CMake, and AutoGen are needed for the `tools/android-prepare-api.sh` script.

##### Ubuntu version

No specific Ubuntu version is needed. The scripts should work if all the
dependencies compile with the build tools available on your platform, any
platform with recent versions of Autotools and CMake should do. The scripts were
tested with ubuntu 18.

Dependencies and APEX binaries
------------------------------

To crosscompile all the dependencies the `tools/android-prepare-api.sh` script
is provided.

Most dependencies can be build with cmake and the cmake toolchain provided by the NDK. Other dependencies not using cmake need to be build with instructions described [here](https://developer.android.com/ndk/guides/other_build_systems) and [here](https://android.googlesource.com/platform/ndk/+/HEAD/docs/BuildSystemMaintainers.md). Since NDK r17 care should be taken not to include symbols from `libgcc.a` and `libunwind.a` to avoid problems with stack unwinding after an exception is thrown. This is described [here](https://android.googlesource.com/platform/ndk/+/HEAD/docs/BuildSystemMaintainers.md#unwinding) in detail.

Libc++ is used as the STL. Libc++ expects type information for exception handling and dynamic casts. Providing a [key funcion](http://itanium-cxx-abi.github.io/cxx-abi/abi.html#vague-vtable) ensures type information. This is an extra requirement for C++ code that should run on Android.

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

1. `--release`: for release.

2. `--ks`: path to the keystore containing the key which the apk will be signed
   with. This key is stored in a JKS keystore.

3. `--ks-pass-path`: this is the path to a file containing the keystore password.

APKs need to be signed so the user can verify that any next version is issued by
us. The public key certificate included with the APK is self-signed. The JKS
keystore and password should be kept in a safe location. Only the Android build
server should have access to these.

F-Droid
-------

To publish the apk to the fdroid repo, simply copy it to the repository folder
and call the deploy script, see the [release
guide](apex-release-guide.md) for more information.
