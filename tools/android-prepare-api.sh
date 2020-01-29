#!/bin/bash -e

cd "$(dirname $(readlink -f ${BASH_SOURCE[0]}))/.."

ROOTDIR=$(pwd)
APIDIR=$ROOTDIR/.build/api-android
ANDROID_API_LEVEL=
JOBS=1

source tools/android-versions.sh

parsecmd() {
    while [ $# -gt 0 ]; do
        #CMDSTART
        case "$1" in
        --api-dir) #
            # set the directory where all the needed libraries will be installed
            APIDIR=${2%/}
            shift
            ;;
        --android-api-level) # [16|...]
            # use a specific android platform api level instead of the default one
            ANDROID_API_LEVEL=$2
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

################# armv7
TARGET_HOST=arm-linux-androideabi
ANDROID_ABI=armeabi-v7a
COMPILER_PREFIX_TRIPLE=armv7a-linux-androideabi
BINUTILS_PREFIX_TRIPLE=arm-linux-androideabi
OPENSSL_ARCHITECTURE=android-arm
ANDROID_PLATFORM_LIB_FOLDER=arch-arm
INSTALL_PREFIX=$APIDIR/host/armv7/usr

################# x86
#TARGET_HOST=x86-linux-android
#ANDROID_ABI=x86
#COMPILER_PREFIX_TRIPLE=i686-linux-android
#BINUTILS_PREFIX_TRIPLE=$COMPILER_PREFIX_TRIPLE
#OPENSSL_ARCHITECTURE=android-x86
#ANDROID_PLATFORM_LIB_FOLDER=arch-x86
#INSTALL_PREFIX=$APIDIR/host/x86/usr

################# setup NDK autoconf environment variables
export ANDROID_NDK_HOME=$APIDIR/android-ndk-${ANDROID_NDK_VERSION}
TOOLCHAIN=$ANDROID_NDK_HOME/toolchains/llvm/prebuilt/linux-x86_64
export AR=$TOOLCHAIN/bin/${BINUTILS_PREFIX_TRIPLE}-ar
export AS=$TOOLCHAIN/bin/${BINUTILS_PREFIX_TRIPLE}-as
export CC=$TOOLCHAIN/bin/${COMPILER_PREFIX_TRIPLE}${ANDROID_API_LEVEL}-clang
export CXX=$TOOLCHAIN/bin/${COMPILER_PREFIX_TRIPLE}${ANDROID_API_LEVEL}-clang++
export LD=$TOOLCHAIN/bin/${BINUTILS_PREFIX_TRIPLE}-ld
export RANLIB=$TOOLCHAIN/bin/${BINUTILS_PREFIX_TRIPLE}-ranlib
export STRIP=$TOOLCHAIN/bin/${BINUTILS_PREFIX_TRIPLE}-strip

################# setup pkg-config
export PKG_CONFIG_LIBDIR=$INSTALL_PREFIX/lib/pkgconfig

################# android NDK
mkdir -p $APIDIR
pushd $APIDIR
[ -f android-ndk-${ANDROID_NDK_VERSION}-linux-x86_64.zip ] || wget https://dl.google.com/android/repository/android-ndk-${ANDROID_NDK_VERSION}-linux-x86_64.zip
[ -d android-ndk-${ANDROID_NDK_VERSION} ] || unzip -q android-ndk-${ANDROID_NDK_VERSION}-linux-x86_64.zip
popd

################# android SDK
mkdir -p $APIDIR
pushd $APIDIR
[ -f android-sdk-linux-${ANDROID_SDK_VERSION}.zip ] || wget https://dl.google.com/android/repository/sdk-tools-linux-${ANDROID_SDK_VERSION}.zip -O android-sdk-linux-${ANDROID_SDK_VERSION}.zip
if [ ! -d android-sdk-linux ]; then
    mkdir android-sdk-linux
    pushd android-sdk-linux
    unzip -q ../android-sdk-linux-${ANDROID_SDK_VERSION}.zip
    yes | ./tools/bin/sdkmanager "build-tools;${ANDROID_BUILD_TOOLS_VERSION}" "platforms;android-${ANDROID_API_LEVEL}" "platform-tools"
    popd
fi
popd

################# Qt
pushd $APIDIR
QT_VERSION_MAJOR=$(echo $QT_VERSION | cut -d '.' -f 1,2)
[ -f qt-opensource-linux-x64-${QT_VERSION}.run ] || wget https://download.qt.io/archive/qt/${QT_VERSION_MAJOR}/${QT_VERSION}/qt-opensource-linux-x64-${QT_VERSION}.run
if [ ! -d Qt ]; then
    QT_VERSION_INSTALL_SCRIPT=$(echo $QT_VERSION | cut -d '.' -f 1,2,3 --output-delimiter='')
    sed "s/QT_VERSION/$QT_VERSION_INSTALL_SCRIPT/g" $ROOTDIR/tools/non-interactive-qt.qs > non-interactive-qt.qs
    chmod +x qt-opensource-linux-x64-${QT_VERSION}.run
    xvfb-run -a ./qt-opensource-linux-x64-${QT_VERSION}.run --script non-interactive-qt.qs
    rm non-interactive-qt.qs
fi
popd

################# xml2
mkdir -p $APIDIR/xml2
pushd $APIDIR/xml2
[ -d libxml2 ] || git clone https://gitlab.gnome.org/GNOME/libxml2.git
if [  ! -f $INSTALL_PREFIX/lib/libxml2.so ]; then
    pushd libxml2
    git checkout -q $XML2_TAG
    ./autogen.sh --prefix=$INSTALL_PREFIX --host=$TARGET_HOST --without-python --enable-static=no --disable-silent-rules
    # some symbols should be hidden to avoid problems with stack unwinding after an exception is thrown
    # see https://android.googlesource.com/platform/ndk/+/ndk-release-r21/docs/BuildSystemMaintainers.md
    # for more information
    # -Xlinker is required for passing linker options unaltered through the libtool build
    make -j "$JOBS" LDFLAGS="-Xlinker --exclude-libs,libgcc_real.a -Xlinker --exclude-libs,libunwind.a" install
    make clean
    popd
fi
popd

################# portaudio
mkdir -p $APIDIR/portaudio
pushd $APIDIR/portaudio
[ -d  portaudio_opensles ] || git clone https://github.com/Gundersanne/portaudio_opensles.git
if [ ! -f $INSTALL_PREFIX/lib/libportaudio.so ]; then
    pushd portaudio_opensles
    git checkout -q $PORTAUDIO_OPENSLES_TAG
    ./configure --prefix=$INSTALL_PREFIX --host=$TARGET_HOST --with-opensles
    make -j "$JOBS" install
    make clean
    popd
fi
popd

################# libsndfile
mkdir -p $APIDIR/sndfile
pushd $APIDIR/sndfile
[ -d libsndfile ] || git clone https://github.com/erikd/libsndfile
if [ ! -f $INSTALL_PREFIX/lib/libsndfile.so ]; then
    pushd libsndfile
    git checkout -q $SNDFILE_TAG
    ./autogen.sh
    ./configure --prefix=$INSTALL_PREFIX --host=$TARGET_HOST --enable-static=no
    # some symbols should be hidden to avoid problems with stack unwinding after an exception is thrown
    # see https://android.googlesource.com/platform/ndk/+/ndk-release-r21/docs/BuildSystemMaintainers.md
    # for more information
    make -j "$JOBS" LDFLAGS="-Wl,--exclude-libs,libgcc_real.a -Wl,--exclude-libs,libunwind.a" install
    make clean
    popd
fi
popd

################# fftw
mkdir -p $APIDIR/fftw3
pushd $APIDIR/fftw3
[ -f fftw-${FFTW_VERSION}.tar.gz ] || wget ftp://ftp.fftw.org/pub/fftw/fftw-${FFTW_VERSION}.tar.gz
[ -d fftw3 ] || (tar -xf fftw-${FFTW_VERSION}.tar.gz && mv fftw-${FFTW_VERSION} fftw3)
if [ ! -f $INSTALL_PREFIX/lib/libfftw3f.so ]; then
    pushd fftw3

    mkdir -p build
    cmake \
        -S . \
        -B build \
        -DCMAKE_TOOLCHAIN_FILE=$ANDROID_NDK_HOME/build/cmake/android.toolchain.cmake \
        -DCMAKE_BUILD_TYPE=Release \
        -DANDROID_ABI=$ANDROID_ABI \
        -DANDROID_PLATFORM=$ANDROID_API_LEVEL \
        -DCMAKE_INSTALL_PREFIX=$INSTALL_PREFIX \
        -DBUILD_SHARED_LIBS=ON \
        -DBUILD_TESTS=OFF \
        -DENABLE_FLOAT=ON
    cmake --build build --target install -j "$JOBS"
    mv $INSTALL_PREFIX/lib/pkgconfig/fftwf.pc $INSTALL_PREFIX/lib/pkgconfig/fftw3f.pc
    rm -rf build

    popd
fi
if [ ! -f $INSTALL_PREFIX/lib/libfftw3.so ]; then
    pushd fftw3

    mkdir -p build
    cmake \
        -S . \
        -B build \
        -DCMAKE_TOOLCHAIN_FILE=$ANDROID_NDK_HOME/build/cmake/android.toolchain.cmake \
        -DCMAKE_BUILD_TYPE=Release \
        -DANDROID_ABI=$ANDROID_ABI \
        -DANDROID_PLATFORM=$ANDROID_API_LEVEL \
        -DCMAKE_INSTALL_PREFIX=$INSTALL_PREFIX \
        -DBUILD_SHARED_LIBS=ON \
        -DBUILD_TESTS=OFF \
        -DENABLE_FLOAT=OFF
    cmake --build build --target install -j "$JOBS"
    mv $INSTALL_PREFIX/lib/pkgconfig/fftw.pc $INSTALL_PREFIX/lib/pkgconfig/fftw3.pc
    rm -rf build

    popd
fi
popd

################# openssl
mkdir -p $APIDIR/openssl
pushd $APIDIR/openssl
[ -d openssl ] || git clone https://github.com/openssl/openssl.git
if [ ! -f $INSTALL_PREFIX/lib/libssl${OPENSSL_SO_SUFFIX}.so ]; then
    pushd openssl
    git checkout -q $OPENSSL_TAG

    PATH=$ANDROID_NDK_HOME/toolchains/llvm/prebuilt/linux-x86_64/bin:$PATH
    ./Configure no-ssl2 no-ssl3 no-comp no-hw no-engine -D__ANDROID_API__=$ANDROID_API_LEVEL --prefix=$INSTALL_PREFIX --openssldir=$INSTALL_PREFIX/ssl $OPENSSL_ARCHITECTURE

    make depend
    make -j "$JOBS" SHLIB_VERSION_NUMBER= SHLIB_EXT=${OPENSSL_SO_SUFFIX}.so build_libs
    cp lib{crypto,ssl}${OPENSSL_SO_SUFFIX}.so $INSTALL_PREFIX/lib
    cp -rL include/openssl $INSTALL_PREFIX/include
    cp *.pc $INSTALL_PREFIX/lib/pkgconfig
    sed -i 's/-lssl/-lssl_1_1/' $INSTALL_PREFIX/lib/pkgconfig/libssl.pc
    sed -i 's/-lcrypto/-lcrypto_1_1/' $INSTALL_PREFIX/lib/pkgconfig/libcrypto.pc
    make clean
    popd
fi
popd

################# libssh2
mkdir -p $APIDIR/libssh2
pushd $APIDIR/libssh2
[ -d libssh2 ] || git clone https://github.com/libssh2/libssh2
if [ ! -f $INSTALL_PREFIX/lib/libssh2.so ]; then
    pushd libssh2
    git checkout -q $SSH2_TAG

    mkdir -p build

    cmake \
        -S . \
        -B build \
        -DCMAKE_TOOLCHAIN_FILE=$ANDROID_NDK_HOME/build/cmake/android.toolchain.cmake \
        -DCMAKE_BUILD_TYPE=Release \
        -DANDROID_ABI=$ANDROID_ABI \
        -DANDROID_PLATFORM=$ANDROID_API_LEVEL \
        -DCMAKE_INSTALL_PREFIX=$INSTALL_PREFIX \
        -DCRYPTO_BACKEND=OpenSSL \
        -DOPENSSL_SSL_LIBRARY=$INSTALL_PREFIX/lib/libssl${OPENSSL_SO_SUFFIX}.so \
        -DOPENSSL_CRYPTO_LIBRARY=$INSTALL_PREFIX/lib/libcrypto${OPENSSL_SO_SUFFIX}.so \
        -DOPENSSL_INCLUDE_DIR=$INSTALL_PREFIX/include \
        -DENABLE_ZLIB_COMPRESSION=ON \
        -DZLIB_LIBRARY=$ANDROID_NDK_HOME/platforms/android-$ANDROID_API_LEVEL/$ANDROID_PLATFORM_LIB_FOLDER/usr/lib/libz.so \
        -DBUILD_EXAMPLES=OFF \
        -DBUILD_TESTING=OFF \
        -DBUILD_SHARED_LIBS=ON
    cmake --build build --target install -j "$JOBS"
    sed -i 's/,zlib//' $INSTALL_PREFIX/lib/pkgconfig/libssh2.pc
    rm -rf build

    popd
fi
popd

################# libgit2
mkdir -p $APIDIR/libgit2
pushd $APIDIR/libgit2
[ -d libgit2 ] || git clone https://github.com/libgit2/libgit2
if [ ! -f $INSTALL_PREFIX/lib/libgit2.so ]; then
    pushd libgit2
    git checkout -q $LIBGIT2_TAG

    mkdir -p build

    cmake \
        -S . \
        -B build \
        -DCMAKE_TOOLCHAIN_FILE=$ANDROID_NDK_HOME/build/cmake/android.toolchain.cmake \
        -DCMAKE_BUILD_TYPE=Release \
        -DANDROID_ABI=$ANDROID_ABI \
        -DANDROID_PLATFORM=$ANDROID_API_LEVEL \
        -DCMAKE_INSTALL_PREFIX=$INSTALL_PREFIX \
        -DOPENSSL_SSL_LIBRARY=$INSTALL_PREFIX/lib/libssl${OPENSSL_SO_SUFFIX}.so \
        -DOPENSSL_CRYPTO_LIBRARY=$INSTALL_PREFIX/lib/libcrypto${OPENSSL_SO_SUFFIX}.so \
        -DOPENSSL_INCLUDE_DIR=$INSTALL_PREFIX/include \
        -DZLIB_LIBRARY=$ANDROID_NDK_HOME/platforms/android-$ANDROID_API_LEVEL/$ANDROID_PLATFORM_LIB_FOLDER/usr/lib/libz.so \
        -DBUILD_CLAR=OFF
    cmake --build build --target install -j "$JOBS"
    sed -i 's/zlib//' $INSTALL_PREFIX/lib/pkgconfig/libgit2.pc
    rm -rf build

    popd
fi
popd
