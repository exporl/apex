#### Prerequisites

The libraries and tools necessary for apex are built in the
android-prepare-api.sh and android-buildprepare-api.shscripts. Some configure
scripts or source files had to be modified, these are explained in comments in
the script.

###### A note on Qt 5.6 for android

Because Qt 5.7 for android requires C++11 support, and the ndk used does not support C++11, Qt 5.6 is required. Ndk r9b is used, newer versions can't build some of the libraries required.

###### Java 32bit

Qt for android needs a 32bit java jdk. The script retrieves the location from
updata-java-alternatives. If no i386 installation is found, no apk can be built and installed,
but apex will build.

---
#### Building

To build simply run tools/android-prepare-api.sh and tools/android-build.sh.

---
### Todo

+ The android-build script can build an apk. For now however, you need to make some symbolic links inside the lib directory on the android device as described in the build script.

+ The app crashes with fatal signal from libc, code -6.

+ Set the ANDROID_PACKAGE_SOURCE_DIR to a directory containing a custom AndroidManifest.xml to specifiy app name etc... (http://doc.qt.io/qt-5/deployment-android.html).

+ Maybe we can use the r9b ndk to build the libraries which fail to build with newer versions, and a newer ndk to build the remaining libraries and apex.
