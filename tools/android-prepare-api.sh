#!/bin/bash -e

cd "$(dirname $(readlink -f ${BASH_SOURCE[0]}))/.."

ROOTDIR=$(pwd)
APIDIR=$ROOTDIR/.build/api-android
ANDROID_PLATFORM=android-14
JOBS=1
JAVA_HOME=/usr/lib/jvm/default-java

parsecmd() {
    while [ $# -gt 0 ]; do
        #CMDSTART
        case "$1" in
        --api-dir) #
            # set the directory where all the needed libraries will be installed
            APIDIR=${2%/}
            shift
            ;;
        --android-platform) # [android-14|...]
            # use a specific android platform target instead of the default one
            ANDROID_PLATFORM=$2
            shift
            ;;
        --java-home) #
            # set the java vm home directory
            JAVA_HOME=$2
            shift
            ;;
        -h|--help) #
            # this help
            echo "Usage: $0 [OPTION]..."
            printf "\nCommand line arguments:\n"
            sed -rn '/CMDSTART/,/CMDEND/{/\) \#|^ +# /{s/\)? #//g;s/^    //;p}}' "$0"
            exit 0
            ;;
        -j|--jobs) # [jobs]
            # specifies the number of jobs to run simultaneously
            JOBS=$2
            shift
            ;;
        *)
            echo "Unknown parameter $1"
            exit 1
            ;;
        esac
        #CMDEND
        shift
    done
}

parsecmd "$@"

mkdir -p $APIDIR/icu
mkdir -p $APIDIR/protobuf
mkdir -p $APIDIR/android
mkdir -p $APIDIR/build_libs/usr
mkdir -p $APIDIR/host/armv7/usr/include
mkdir -p $APIDIR/host/armv7/usr/lib
mkdir -p $APIDIR/host/x86/usr/include
mkdir -p $APIDIR/host/x86/usr/lib

BUILD_LIBS=$APIDIR/build_libs/usr
HOST_ARMV7_LIBS=$APIDIR/host/armv7/usr
HOST_X86_LIBS=$APIDIR/host/x86/usr

pushd $APIDIR
# android ndk used to build the libraries
pushd android
[ -f android-ndk-r9b-linux-x86_64.tar.bz2 ] || wget -q http://dl.google.com/android/ndk/android-ndk-r9b-linux-x86_64.tar.bz2
[ -d android-ndk-r9b ] || tar -xf android-ndk-r9b-linux-x86_64.tar.bz2
# standalone toolchain we use to crosscompile the libraries
[ -d android_standalone_armv7/bin ] || android-ndk-r9b/build/tools/make-standalone-toolchain.sh --platform=$ANDROID_PLATFORM --install-dir=$APIDIR/android/android_standalone_armv7 --toolchain=arm-linux-androideabi-4.8
[ -d android_standalone_x86/bin ] || android-ndk-r9b/build/tools/make-standalone-toolchain.sh --platform=$ANDROID_PLATFORM --install-dir=$APIDIR/android/android_standalone_x86 --toolchain=x86-4.8
# android sdk
[ -f tools_r25.2.5-linux.zip ] || wget -q https://dl.google.com/android/repository/tools_r25.2.5-linux.zip
if [ ! -d android-sdk-linux ]; then
    mkdir android-sdk-linux
    pushd android-sdk-linux
    unzip ../tools_r25.2.5-linux.zip 1>/dev/null
    yes | ./tools/bin/sdkmanager "build-tools;24.0.3" "platforms;android-14" "platform-tools"
    popd
fi
popd

# icu
# to crosscompile icu we need a local build
pushd icu
[ -f icu4c-57_1-src.tgz ] || wget -q http://download.icu-project.org/files/icu4c/57.1/icu4c-57_1-src.tgz
[ -d icu_localbuild ] || (tar -xf icu4c-57_1-src.tgz && mv icu icu_localbuild)

if [ ! -f icu_localbuild/source/lib/libicuio.so ]; then
    pushd icu_localbuild && pushd source
    ./configure
    make -j "$JOBS"
    popd && popd
fi
popd

# protobuf
pushd protobuf
[ -f protobuf-2.6.1.tar.gz ] || wget -q https://github.com/google/protobuf/releases/download/v2.6.1/protobuf-2.6.1.tar.gz
[ -d protobuf ] || (tar -xf protobuf-2.6.1.tar.gz && mv protobuf-2.6.1 protobuf)
if [ ! -f $BUILD_LIBS/bin/protoc ]; then
    pushd protobuf
    ./autogen.sh
    sed -i 's/\(library_names_spec=.\|soname_spec=.\).{libname}.{.*\(.\)/\1${libname}${shared_ext}\2/' configure
    ./configure --prefix=$BUILD_LIBS
    make -j "$JOBS" install
    make clean
    popd
fi
popd

# end of BUILD (local system) builds
# start of crosscompile builds

setarmv7env() {
    ANDROID_NDK_ROOT=$APIDIR/android/android_standalone_armv7
    ANDROID_TARGET_ARCH=armeabi-v7a
    ANDROID_NDK_TOOLCHAIN_PREFIX=arm-linux-androideabi
    TARGET_HOST=arm-linux-androideabi
    ANDROID_ARCHITECTURE=armv7
    ANDROID_NDK_TOOLCHAIN_VERSION=4.8
    ANDROID_NDK_HOST=linux-x86_64
    SYSROOT=$ANDROID_NDK_ROOT/sysroot
    PKG_CONFIG_LIBDIR=$HOST_ARMV7_LIBS/lib/pkgconfig

    export ANDROID_NDK_ROOT ANDROID_TARGET_ARCH ANDROID_NDK_TOOLCHAIN_PREFIX ANDROID_NDK_TOOLCHAIN_VERSION SYSROOT JAVA_HOME PKG_CONFIG_LIBDIR

    ANDROID_NDK_TOOLS_FULLPREFIX=$ANDROID_NDK_ROOT/bin/$ANDROID_NDK_TOOLCHAIN_PREFIX
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
    LIBS="-lc -lgcc -L$SYSROOT/usr/lib -I$HOST_ARMV7_LIBS/include -L$HOST_ARMV7_LIBS/lib -I$ANDROID_NDK_ROOT/include/c++/$ANDROID_NDK_TOOLCHAIN_VERSION -L$ANDROID_NDK_ROOT/$ANDROID_NDK_TOOLCHAIN_PREFIX/lib/armv7-a"
    CPPFLAGS="--sysroot=$SYSROOT -I$HOST_ARMV7_LIBS/include"
    CXXFLAGS="--sysroot=$SYSROOT -I$HOST_ARMV7_LIBS/include"
    LDFLAGS="-lc -lgnustl_shared"

    export CPPFLAGS LIBS CC CXX CPP AR STRIP RANLIB LD AS LDFLAGS LIBS CFLAGS

    CURRENT_PREFIX=$HOST_ARMV7_LIBS

    export OPENSSL_CROSSCOMPILE=android-armv7
}

setx86env() {
    ANDROID_NDK_ROOT=$APIDIR/android/android_standalone_x86
    ANDROID_TARGET_ARCH=i686-linux-android
    ANDROID_NDK_TOOLCHAIN_PREFIX=i686-linux-android
    TARGET_HOST=x86-linux-android
    ANDROID_NDK_TOOLCHAIN_VERSION=4.8
    ANDROID_NDK_HOST=linux-x86_64
    SYSROOT=$ANDROID_NDK_ROOT/sysroot
    PKG_CONFIG_LIBDIR=$HOST_X86_LIBS/lib/pkgconfig

    export ANDROID_NDK_ROOT ANDROID_TARGET_ARCH ANDROID_NDK_TOOLCHAIN_PREFIX ANDROID_NDK_TOOLCHAIN_VERSION SYSROOT JAVA_HOME PKG_CONFIG_LIBDIR

    ANDROID_NDK_TOOLS_FULLPREFIX=$ANDROID_NDK_ROOT/bin/$ANDROID_NDK_TOOLCHAIN_PREFIX
    CC="$ANDROID_NDK_TOOLS_FULLPREFIX-gcc"
    CXX="$ANDROID_NDK_TOOLS_FULLPREFIX-g++"
    CPP="$ANDROID_NDK_TOOLS_FULLPREFIX-cpp"
    AR="$ANDROID_NDK_TOOLS_FULLPREFIX-ar"
    STRIP="$ANDROID_NDK_TOOLS_FULLPREFIX-strip"
    RANLIB="$ANDROID_NDK_TOOLS_FULLPREFIX-ranlib"
    LD="$ANDROID_NDK_TOOLS_FULLPREFIX-ld"
    AS="$ANDROID_NDK_TOOLS_FULLPREFIX-as"

    CFLAGS="-g -O2"
    LIBGCC_PATH_FULL=$($CC -print-libgcc-file-name)
    LIBGCC_PATH=$(dirname "$LIBGCC_PATH_FULL")
    LIBS="-lc -lgcc -L$SYSROOT/usr/lib -I$HOST_X86_LIBS/include -L$HOST_X86_LIBS/lib -I$ANDROID_NDK_ROOT/include/c++/$ANDROID_NDK_TOOLCHAIN_VERSION -L$ANDROID_NDK_ROOT/$ANDROID_NDK_TOOLCHAIN_PREFIX/lib"
    CPPFLAGS="--sysroot=$SYSROOT -I$HOST_X86_LIBS/include"
    CXXFLAGS="--sysroot=$SYSROOT -I$HOST_X86_LIBS/include"

    export CPPFLAGS LIBS CC CXX CPP AR STRIP RANLIB LD AS LDFLAGS LIBS CFLAGS
    CURRENT_PREFIX=$HOST_X86_LIBS

    export OPENSSL_CROSSCOMPILE=android-x86
}

dobuild() {
    # building glob for xml2, this expexts glob.c and glob.h in the glob directory
    # glob is not part of the bionic stdlib
    [ -f $ROOTDIR/tools/glob/glob.h ] || (echo "glob.h not present" && exit)
    [ -f $ROOTDIR/tools/glob/glob.c ] || (echo "glob.c not present" && exit)
    cp $ROOTDIR/tools/glob/glob.h $CURRENT_PREFIX/lib/
    cp $ROOTDIR/tools/glob/glob.h $CURRENT_PREFIX/include/
    cp $ROOTDIR/tools/glob/glob.c $CURRENT_PREFIX/lib/
    rm -f $CURRENT_PREFIX/lib/glob.o
    $CC -c -I$CURRENT_PREFIX/include/ $CURRENT_PREFIX/lib/glob.c -o $CURRENT_PREFIX/lib/glob.o
    chmod +x $CURRENT_PREFIX/lib/glob.o

    export LIBS="$LIBS $CURRENT_PREFIX/lib/glob.o"

    pushd $APIDIR

    # icu transcoder, needed for protobuf
    [ -d icu ] || (echo "icu not present, run android-prebuild first" && exit)
    pushd icu
    [ -f icu4c-57_1-src.tgz ] || (echo "run android-prebuild first" && exit)
    [ -d icu_hostbuild ] || (tar -xf icu4c-57_1-src.tgz && mv icu icu_hostbuild)

    if [ ! -f $CURRENT_PREFIX/lib/libicuio.a ]; then
        pushd icu_hostbuild && pushd source
        ./configure --prefix=$CURRENT_PREFIX --host=$TARGET_HOST --with-cross-build=$APIDIR/icu/icu_localbuild/source --enable-static --disable-shared
        make -j "$JOBS" install
        make clean
        popd && popd
    fi
    popd

    # protobuf
    [ -d protobuf ] || (echo "protobuf not present, run android-prebuild first" && exit)
    [ -f $BUILD_LIBS/bin/protoc ] || (echo "$BUILD_LIBS/bin/protoc not present, run android-prebuild first" && exit)
    pushd protobuf
    [ -d protobuf ] || (echo "protobuf not present, run android-prebuild first" && exit)
    if [ ! -f $CURRENT_PREFIX/bin/protoc ]; then
        pushd protobuf
        sed -i 's/\(library_names_spec=.\|soname_spec=.\).{libname}.{.*\(.\)/\1${libname}${shared_ext}\2/' configure
        ./configure --prefix=$CURRENT_PREFIX --host=$TARGET_HOST --with-protoc=$BUILD_LIBS/bin/protoc
        make -j "$JOBS" install
        make clean
        popd
    fi
    popd

    # xml2
    mkdir -p xml2
    pushd xml2
    [ -d libxml2 ] || git clone -q https://gitlab.gnome.org/GNOME/libxml2.git
    if [  ! -f $CURRENT_PREFIX/lib/libxml2.so ]; then
        pushd libxml2
        git checkout bdec2183f34b37ee89ae1d330c6ad2bb4d76605f
        ./autogen.sh --prefix=$CURRENT_PREFIX --host=$TARGET_HOST --target=$TARGET_HOST --without-python
        sed -i 's/\(library_names_spec=.\|soname_spec=.\).{libname}.{.*\(.\)/\1${libname}${shared_ext}\2/' configure
        make -j "$JOBS" install
        make clean
        popd
    fi
    popd

    # portaudio
    mkdir -p portaudio
    pushd portaudio
    [ -d  portaudio_opensles ] || git clone -q https://github.com/Gundersanne/portaudio_opensles.git
    if [ ! -f $CURRENT_PREFIX/lib/libportaudio.so ]; then
        pushd portaudio_opensles
        sed -i 's/\(library_names_spec=.\|soname_spec=.\).{libname}.{.*\(.\)/\1${libname}${shared_ext}\2/' configure
        ./configure --prefix=$CURRENT_PREFIX --host=$TARGET_HOST --with-opensles
        make -j "$JOBS" install
        make clean
        popd
    fi
    popd

    # libsndfile
    mkdir -p sndfile
    pushd sndfile
    [ -d libsndfile ] || git clone -q https://github.com/erikd/libsndfile
    if [ ! -f $CURRENT_PREFIX/lib/libsndfile.so ]; then
        pushd libsndfile
        git checkout 1.0.27
        ./autogen.sh
        sed -i 's/\(library_names_spec=.\|soname_spec=.\).{libname}.{.*\(.\)/\1${libname}${shared_ext}\2/' configure
        ./configure --prefix=$CURRENT_PREFIX --host=$TARGET_HOST
        make -j "$JOBS" install
        make clean
        popd
    fi
    popd

    # fftw3f and fftw3
    mkdir -p fftw3
    pushd fftw3
    [ -f fftw-3.3.5.zip ] || wget -q ftp://ftp.fftw.org/pub/fftw/fftw-3.3.5.zip
    [ -d fftw3 ] || (unzip fftw-3.3.5.zip && mv fftw-3.3.5 fftw3)
    if [ ! -f $CURRENT_PREFIX/lib/libfftw3f.so ]; then
        pushd fftw3
        sed -i 's/\(library_names_spec=.\|soname_spec=.\).{libname}.{.*\(.\)/\1${libname}${shared_ext}\2/' configure
        ./configure --prefix=$CURRENT_PREFIX --host=$TARGET_HOST --enable-float --with-slow-timer --enable-shared
        make -j "$JOBS" install
        make clean
        popd
    fi
    if [ ! -f $CURRENT_PREFIX/lib/libfftw3.so ]; then
        pushd fftw3
        sed -i 's/\(library_names_spec=.\|soname_spec=.\).{libname}.{.*\(.\)/\1${libname}${shared_ext}\2/' configure
        ./configure --prefix=$CURRENT_PREFIX --host=$TARGET_HOST --with-slow-timer --enable-shared
        make -j "$JOBS" install
        make clean
        popd
    fi
    popd

    # libssl
    mkdir -p openssl
    pushd openssl
    [ -d openssl ] || git clone -q https://github.com/openssl/openssl.git
    if [ ! -f $CURRENT_PREFIX/lib/libssl.a ]; then
        pushd openssl
        git checkout -q OpenSSL_1_0_2-stable
        ./Configure no-ssl3 no-shared \
                    --prefix=$CURRENT_PREFIX \
                    --openssldir=$CURRENT_PREFIX/ssl \
                    --sysroot=$SYSROOT \
                    $OPENSSL_CROSSCOMPILE
        make depend
        make -j "$JOBS" all

             # CALC_VERSIONS="SHLIB_COMPAT=; SHLIB_SOVER=" all
        make install # INSTALL_SHLIBS="libcrypto.so libssl.so" \
             # INSTALL_SHLIB_INFO=
        make clean
        popd
    fi
    popd

    # libssh2
    mkdir -p libssh2
    pushd libssh2
    [ -d libssh2 ] || git clone -q https://github.com/libssh2/libssh2
    if [ ! -f $CURRENT_PREFIX/lib/libssh2.a ]; then
        pushd libssh2
        git checkout -q libssh2-1.8.0
        rm -f configure
        if [ ! -f configure ]; then
            sed -i 's/example\/libssh2_config.h//' configure.ac
            libtoolize --force
            aclocal
            autoheader
            automake --force-missing --add-missing
            autoconf
        fi
        sed -i 's/\(library_names_spec=.\|soname_spec=.\).{libname}.{.*\(.\)/\1${libname}${shared_ext}\2/' configure
        ./configure --prefix=$CURRENT_PREFIX --host=$TARGET_HOST \
                    CFLAGS="${CFLAGS//-nostdlib/}" LIBS="$LIBS -lssl -lcrypto" \
                    --disable-examples-build \
                    --with-libz=no \
                    --disable-shared
        make -j "$JOBS" install
        make clean
        popd
    fi
    popd

    # libgit2
    mkdir -p libgit2
    pushd libgit2
    [ -d libgit2 ] || git clone -q https://github.com/libgit2/libgit2
    if [ ! -f $CURRENT_PREFIX/lib/libgit2.so ]; then
        pushd libgit2
        git checkout -q v0.26.0

        touch CMakeToolchain.txt
        : > CMakeToolchain.txt
        echo "SET(CMAKE_SYSTEM_NAME Linux)" >> CMakeToolchain.txt
        echo "SET(CMAKE_SYSTEM_VERSION Android)" >> CMakeToolchain.txt
        echo "SET(CMAKE_C_COMPILER   $CC)" >> CMakeToolchain.txt
        echo "SET(CMAKE_CXX_COMPILER $CXX)" >> CMakeToolchain.txt
        echo "SET(CMAKE_FIND_ROOT_PATH $SYSROOT)" >> CMakeToolchain.txt
        echo "SET(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)" >> CMakeToolchain.txt
        echo "SET(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)" >> CMakeToolchain.txt
        echo "SET(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)" >> CMakeToolchain.txt

        mkdir -p build
        pushd build
        cmake ../ -DCMAKE_TOOLCHAIN_FILE=../CMakeToolchain.txt \
              -DCMAKE_INSTALL_PREFIX=$CURRENT_PREFIX -DSONAME=OFF \
              -DCMAKE_C_FLAGS="$CFLAGS $LIBS -lssl -lcrypto -lssh2 -ldl" \
              -DOPENSSL_SSL_LIBRARY=$CURRENT_PREFIX/lib/libssl.a \
              -DOPENSSL_CRYPTO_LIBRARY=$CURRENT_PREFIX/lib/libcrypto.a \
              -DOPENSSL_INCLUDE_DIR=$CURRENT_PREFIX/include \
              -DZLIB_LIBRARY=$SYSROOT/usr/lib/libz.so \
              -DBUILD_CLAR=OFF
        sed -i 's/zlib//' libgit2.pc
        cmake --build . --target install
        popd
        rm -rf build
        popd
    fi
    popd
}

setarmv7env
dobuild
setx86env
dobuild

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
