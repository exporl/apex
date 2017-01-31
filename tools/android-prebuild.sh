#!/bin/bash -e

mkdir -p .build/android/extra_x86
mkdir -p .build/android/icu
mkdir -p .build/android/protobuf
mkdir -p .build/android/android
mkdir -p .build/android/extra
mkdir -p .build/android/extra/usr
mkdir -p .build/android/extra/usr/include
mkdir -p .build/android/extra/usr/lib

# script local variables
ROOTDIR=$(pwd)
BUILD_ANDROID=$ROOTDIR/.build/android
EXTRAX86=$BUILD_ANDROID/extra_x86
EXTRA=$BUILD_ANDROID/extra/usr

pushd .build
pushd android

# android ndk used to build the libraries
pushd android
[ -f android-ndk-r9b-linux-x86_64.tar.bz2 ] || wget -q http://dl.google.com/android/ndk/android-ndk-r9b-linux-x86_64.tar.bz2
[ -d android-ndk-r9b ] || tar -xf android-ndk-r9b-linux-x86_64.tar.bz2
# standalone toolchain we use to crosscompile the libraries
mkdir -p android_standalone
[ -d android_standalone/bin ] || android-ndk-r9b/build/tools/make-standalone-toolchain.sh --platform=android-14 --install-dir=$BUILD_ANDROID/android/android_standalone --toolchain=arm-linux-androideabi-4.8
popd

# icu
# to crosscompile icu we need a local build
pushd icu
[ -f icu4c-57_1-src.tgz ] || wget -q http://download.icu-project.org/files/icu4c/57.1/icu4c-57_1-src.tgz
[ -d icu_localbuild ] || (tar -xf icu4c-57_1-src.tgz && mv icu icu_localbuild)

if [ ! -f icu_localbuild/source/lib/libicuio.so ]; then
    pushd icu_localbuild && pushd source
    ./configure
    make -j4
    popd && popd
fi
popd


# protobuf
# to crosscompile protobuf we need a local build
# while building apex makefiles are generated which use protoc, which defaults to the system protoc.
# This is ok, because we couldn't use a crosscompiled protoc. Sadly there isn't a convenient way to
# change the path of this protoc to our local x86 version. This means that the version
# of the crosscompiled protobuf and the local/system protobuf need to be roughly the same.
PROTOC_VERSION=$(protoc --version)
pushd protobuf
if [[ $PROTOC_VERSION == *" 3."* ]]; then
    [ -d protobuf ] || git clone -q https://github.com/google/protobuf.git
    pushd protobuf
    git checkout 137d6a09bbbbfa801d653224703ddc59e3700704
    popd
fi
if [[ $PROTOC_VERSION == *" 2."* ]]; then
    [ -f protobuf-2.5.0.tar.gz ] || wget -q https://github.com/google/protobuf/releases/download/v2.5.0/protobuf-2.5.0.tar.gz
    [ -d protobuf ] || (tar -xf protobuf-2.5.0.tar.gz && mv protobuf-2.5.0 protobuf)
fi
if [ ! -f $EXTRAX86/bin/protoc ]; then
    pushd protobuf
    ./autogen.sh
    sed -i 's/\(library_names_spec=.\|soname_spec=.\).{libname}.{.*\(.\)/\1${libname}${shared_ext}\2/' configure
    ./configure --prefix=$EXTRAX86
    make -j4 install
    make clean
    popd
fi
popd
popd
popd

# end of x86 builds
# start of armv7 builds

ANDROID_NDK_ROOT=$BUILD_ANDROID/android/android_standalone
ANDROID_TARGET_ARCH=armeabi-v7a
ANDROID_NDK_TOOLS_PREFIX=arm-linux-androideabi
ANDROID_NDK_TOOLCHAIN_PREFIX=arm-linux-androideabi
ANDROID_ARCHITECTURE=arm
ANDROID_NDK_TOOLCHAIN_VERSION=4.8
ANDROID_NDK_HOST=linux-x86_64
SYSROOT=$ANDROID_NDK_ROOT/sysroot
PKG_CONFIG_LIBDIR=$EXTRA/lib/pkgconfig

export ANDROID_NDK_ROOT ANDROID_TARGET_ARCH ANDROID_NDK_TOOLS_PREFIX ANDROID_NDK_TOOLCHAIN_PREFIX ANDROID_ARCHITECTURE ANDROID_NDK_TOOLCHAIN_VERSION SYSROOT JAVA_HOME PKG_CONFIG_LIBDIR

ANDROID_NDK_TOOLS_FULLPREFIX=$ANDROID_NDK_ROOT/bin/$ANDROID_NDK_TOOLS_PREFIX

CC="$ANDROID_NDK_TOOLS_FULLPREFIX-gcc"
CXX="$ANDROID_NDK_TOOLS_FULLPREFIX-g++"
CPP="$ANDROID_NDK_TOOLS_FULLPREFIX-cpp"
AR="$ANDROID_NDK_TOOLS_FULLPREFIX-ar"
STRIP="$ANDROID_NDK_TOOLS_FULLPREFIX-strip"
RANLIB="$ANDROID_NDK_TOOLS_FULLPREFIX-ranlib"
LD="$ANDROID_NDK_TOOLS_FULLPREFIX-ld"
AS="$ANDROID_NDK_TOOLS_FULLPREFIX-as"


case "$ANDROID_TARGET_ARCH" in
    armeabi-v7a)
        CFLAGS="-g -O2 -ffunction-sections -funwind-tables -fstack-protector -fno-short-enums -DANDROID -DLITTLE_ENDIAN -Wno-psabi -march=armv7-a -mfloat-abi=softfp -mfpu=vfp -Wa,--noexecstack -nostdlib --sysroot=$SYSROOT"
        ;;
    armeabi)
        CFLAGS="-g -O2 -ffunction-sections -funwind-tables -fstack-protector -fno-short-enums -DANDROID -DLITTLE_ENDIAN -Wno-psabi -march=armv5te -mtune=xscale -msoft-float -Wa,--noexecstack -nostdlib"
        ;;
esac

LIBGCC_PATH_FULL=$($CC -mthumb-interwork -print-libgcc-file-name)
LIBGCC_PATH=$(dirname "$LIBGCC_PATH_FULL")

LIBS="-lc -lgcc -L$SYSROOT/usr/lib -I$EXTRA/include -L$EXTRA/lib -I$ANDROID_NDK_ROOT/include/c++/$ANDROID_NDK_TOOLCHAIN_VERSION -L$ANDROID_NDK_ROOT/$ANDROID_NDK_TOOLCHAIN_PREFIX/lib/armv7-a"
CPPFLAGS="--sysroot=$SYSROOT -I$EXTRA/include"
CXXFLAGS="--sysroot=$SYSROOT -I$EXTRA/include"
LDFLAGS="-lc -lgnustl_shared"

export CPPFLAGS LIBS CC CXX CPP AR STRIP RANLIB LD AS LDFLAGS LIBS CFLAGS

# building glob for xml2, this expexts glob.c and glob.h in the glob directory
# glob is not part of the bionic stdlib
[ -f $ROOTDIR/tools/glob/glob.h ] || (echo "glob.h not present" && exit)
[ -f $ROOTDIR/tools/glob/glob.c ] || (echo "glob.c not present" && exit)
cp $ROOTDIR/tools/glob/glob.h $EXTRA/lib/
cp $ROOTDIR/tools/glob/glob.h $EXTRA/include/
cp $ROOTDIR/tools/glob/glob.c $EXTRA/lib/
rm -f $EXTRA/lib/glob.o
$CC -c -I$EXTRA/include/ $EXTRA/lib/glob.c -o $EXTRA/lib/glob.o
chmod +x $EXTRA/lib/glob.o

export LIBS="$LIBS $EXTRA/lib/glob.o"

pushd .build
pushd android

# icu transcoder, needed for xerces and protobuf
[ -d icu ] || (echo "icu not present, run android-prebuild first" && exit)
pushd icu
[ -f icu4c-57_1-src.tgz ] || (echo "run android-prebuild first" && exit)
[ -d icu_hostbuild ] || (tar -xf icu4c-57_1-src.tgz && mv icu icu_hostbuild)

if [ ! -f $EXTRA/lib/libicuio.a ]; then
    pushd icu_hostbuild && pushd source
#    sed -i 's/\(LIB_VERSION.*\)=.*/\1=/' configure
    ./configure --prefix=$EXTRA --host=arm-linux-androideabi --with-cross-build=$BUILD_ANDROID/icu/icu_localbuild/source --enable-static --disable-shared
    make -j4 install
    popd && popd
fi
popd

export LIBS="$LIBS -licudata -licuio -licule -licuuc -licutu -liculx"

# xerces
# locale is not supported by the bionic stdlib
# in xerces it's used to detect what character should represent a decimal point
# we simply return '.' instead of querying locale
mkdir -p xerces
pushd xerces
[ -f xerces-c-3.1.4.tar.gz ] || wget -q http://archive.apache.org/dist/xerces/c/3/sources/xerces-c-3.1.4.tar.gz
[ -d xerces-c-3.1.4 ] ||  tar -xf xerces-c-3.1.4.tar.gz
if [ ! -f $EXTRA/lib/libxerces-c.so ]; then
    pushd xerces-c-3.1.4
    sed -i '/localeconv()/d' src/xercesc/util/XMLAbstractDoubleFloat.cpp
    sed -i "s/[*]lc->decimal_point/'.'/" src/xercesc/util/XMLAbstractDoubleFloat.cpp
    ./reconf
    ./configure --prefix=$EXTRA --with-icu=$EXTRA/bin --host=arm-linux-androideabi --enable-transcoder-icu
    make -j4 install
    popd
fi
popd

# protobuf
[ -d protobuf ] || (echo "protobuf not present, run android-prebuild first" && exit)
[ -f $EXTRAX86/bin/protoc ] || (echo "$EXTRAX86/bin/protoc not present, run android-prebuild first" && exit)
pushd protobuf
[ -d protobuf ] || (echo "protobuf not present, run android-prebuild first" && exit)
if [ ! -f $EXTRA/bin/protoc ]; then
    pushd protobuf
    sed -i 's/\(library_names_spec=.\|soname_spec=.\).{libname}.{.*\(.\)/\1${libname}${shared_ext}\2/' configure
    ./configure --prefix=$EXTRA --host=arm-linux-androideabi --with-protoc=$EXTRAX86/bin/protoc
    make -j4 install
    make clean
    popd
fi
popd

# xml2
mkdir -p xml2
pushd xml2
[ -d libxml2 ] || git clone -q git://git.gnome.org/libxml2
if [  ! -f $EXTRA/lib/libxml2.so ]; then
    pushd libxml2
    git checkout bdec2183f34b37ee89ae1d330c6ad2bb4d76605f
    ./autogen.sh --prefix=$EXTRA --host=arm-linux-androideabi --target=arm-linux-androideabi --without-python
    sed -i 's/\(library_names_spec=.\|soname_spec=.\).{libname}.{.*\(.\)/\1${libname}${shared_ext}\2/' configure
    make -j4 install
    popd
fi
popd

# jack2
# because binaries are just copied, a packageconfig file needs to be made
# the sedding below simply makes a valid pc file
mkdir -p jack2
pushd jack2
[ -d jack2_android ] || git clone -q https://github.com/KimJeongYeon/jack2_android
if [ ! -f $EXTRA/lib/libjack.so ]; then
    pushd jack2_android
    git checkout 4a8787be4306558cb52e5379466c0ed4cc67e788
    pushd binaries
    [ -d binaries ] || (tar -xf *.tar.gz && mv jack2_android_binary_20140619 binaries)
    cp -r ./binaries/* $EXTRA/
    popd
    EXTRA2=$(echo $EXTRA | sed 's/\//\\\//g')
    cp -r jack/common/jack $EXTRA/include/
    cp jack/jack.pc.in $EXTRA/lib/pkgconfig/jack.pc
    sed -i "s/\(^prefix=\).*/\1$EXTRA2/" $EXTRA/lib/pkgconfig/jack.pc
    sed -i 's/@PREFIX@/${prefix}/g' $EXTRA/lib/pkgconfig/jack.pc
    sed -i 's/@LIBDIR@/${prefix}\/lib/g' $EXTRA/lib/pkgconfig/jack.pc
    sed -i 's/@INCLUDEDIR@/${prefix}\/include/g' $EXTRA/lib/pkgconfig/jack.pc
    sed -i 's/@SERVERLIB@/jackserver/g' $EXTRA/lib/pkgconfig/jack.pc
    sed -i 's/@JACK_VERSION@/1.9.10/g' $EXTRA/lib/pkgconfig/jack.pc
    popd
fi
popd

export LIBS="$LIBS -ljack"

# alsa header files, needed for portaudio
mkdir -p alsaheader
pushd alsaheader
[ -f alsa-lib-1.1.1.tar.bz2 ] || wget -q ftp://ftp.alsa-project.org/pub/lib/alsa-lib-1.1.1.tar.bz2
[ -d alsalib ] || (tar -xf alsa-lib-1.1.1.tar.bz2 && mv alsa-lib-1.1.1 alsalib)
if [ ! -d $EXTRA/include/alsa ]; then
    cp -r alsalib/include/* $EXTRA/include
fi
popd

# portaudio
# the bionic stdlib supports pthread, but in the same way
# no explicit include is necessary and pthread_cancel does not exist
# so we remove the offending lines in the configure script
# and portaudio builds without pthread
mkdir -p portaudio
pushd portaudio
[ -f pa_stable_v19_20140130.tgz ] || wget -q http://www.portaudio.com/archives/pa_stable_v19_20140130.tgz
[ -d portaudio ] || tar -xf *.tgz
if [ ! -f $EXTRA/lib/libportaudio.so ]; then
    pushd portaudio
    sed -i 's/\(.*\)-lpthread\(.*\)/\1\2/' configure
    sed -i 's/.*IRIX posix thread library.*/continue/' configure
    sed -i 's/\(ac_cv_lib_pthread_pthread_create=\)no/\1yes/g' configure
    sed -i 's/\(library_names_spec=.\|soname_spec=.\).{libname}.{.*\(.\)/\1${libname}${shared_ext}\2/' configure
    ./configure --prefix=$EXTRA --host=arm-linux-androideabi --with-jack
    make -j4 install
    popd
fi
popd

# ant
mkdir -p ant
pushd ant
[ -f apache-ant-1.9.7-bin.tar.gz ] || wget -q http://apache.belnet.be//ant/binaries/apache-ant-1.9.7-bin.tar.gz
[ -d apache-ant-1.9.7 ] || tar -xf apache-ant-1.9.7-bin.tar.gz
popd

# Qt 5.6 for android
[ -f qt-opensource-linux-x64-android-5.6.1-1.run ] || wget -q http://download.qt.io/official_releases/qt/5.6/5.6.1-1/qt-opensource-linux-x64-android-5.6.1-1.run
if [ ! -d Qt ]; then
    chmod +x  qt-opensource-linux-x64-android-5.6.1-1.run
    xvfb-run -a ./qt-opensource-linux-x64-android-5.6.1-1.run --script $ROOTDIR/tools/non-interactive-qt.qs
fi

popd
popd
