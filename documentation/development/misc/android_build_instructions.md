#### Prerequisites

The libraries and tools necessary for apex are built in the
android-prepare-api.sh and android-buildprepare-api.sh scripts. Some configure
scripts or source files had to be modified, these are explained in comments in
the script.

###### A note on Qt 5.6 for android

Because Qt 5.7 for android requires C++11 support, and the ndk used does not support C++11, Qt 5.6 is required. Ndk r9b is used, newer versions can't build some of the libraries required.

###### Java 32bit

Qt for android needs a 32bit java jdk. The script retrieves the location from
updata-java-alternatives. If no i386 installation is found, no apk can be built and installed, but apex will build.

###### Android openssl

We compile openssl as a static archive for android. The shared object will conflict with the system
ssl (which is not part of the public api) because of similar symbols.

---
#### Building

To build simply run tools/android-prepare-api.sh and tools/android-build.sh. Autotools and AutoGen are needed.

---
### Todo

+ Upgrade to a newer ndk.
