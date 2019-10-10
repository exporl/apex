# Template for a custom configuration
# You should not need this if you have all dependencies in the right
# directories or you are using Debian GNU/Linux.
#
# The only variable that is defined before is BASEDIR

# Uncomment for release version
#RELEASE = 1

# Uncomment to override make files that want precompiled headers
CLEBS -= pch

# Some echo command lines are too long
CLEBS *= nosilent

# Dependencies for Unix:
MATLABROOT=c:/api-vs2008/matlab71

# Dependencies for Windows
win32:XML2ROOT = c:/api-vs2017/xml2
win32:ICONVROOT = c:/api-vs2017/iconv
win32:ZLIBROOT = c:/api-vs2017/zlib

win32:PORTAUDIOROOT = c:/api-vs2017/portaudio
#win32:PORTAUDIOLIB = portaudio_x86
#win32:PORTAUDIOLIB_RELEASE = $${PORTAUDIOLIB}
#win32:PORTAUDIOLIB_DEBUG = $${PORTAUDIOLIB}
#win32:PORTAUDIOLIBDIR = $${PORTAUDIOROOT}/lib
#win32:PORTAUDIOLIBDIR_RELEASE = $${PORTAUDIOLIBDIR}/release
#win32:PORTAUDIOLIBDIR_DEBUG = $${PORTAUDIOLIBDIR}/debug
#win32:PORTAUDIODLL = $${PORTAUDIOLIB}
#win32:PORTAUDIODLL_RELEASE = $${PORTAUDIODLL}.dll
#win32:PORTAUDIODLL_DEBUG = $${PORTAUDIODLL}.dll
#win32:PORTAUDIODLLDIR = $${PORTAUDIOLIBDIR}
#win32:PORTAUDIODLLDIR_RELEASE = $${PORTAUDIODLLDIR}/release
#win32:PORTAUDIODLLDIR_DEBUG = $${PORTAUDIODLLDIR}/debug
#win32:PORTAUDIOINCLUDEDIR = $${PORTAUDIOROOT}/include

win32:NUCLEUSROOT = c:/api-vs2017/nucleus/c_c++
win32:NUCLEUSLIBDIR = $${NUCLEUSROOT}/libraries/msvc/v7.1
win32:NUCLEUSINCLUDEDIR = $${NUCLEUSROOT}/include

win32:FFTW3ROOT = c:/api-vs2017/fftw3
#win32:FFTW3LIB = libfftw3-3
#win32:FFTW3LIB_RELEASE = $${FFTW3LIB}
#win32:FFTW3LIB_DEBUG = $${FFTW3LIB}
#win32:FFTW3LIBDIR = $${FFTW3ROOT}
#win32:FFTW3LIBDIR_RELEASE = $${FFTW3LIBDIR}
#win32:FFTW3LIBDIR_DEBUG = $${FFTW3LIBDIR}
#win32:FFTW3INCLUDEDIR = $${FFTW3ROOT}

win32:FFTW3FROOT = c:/api-vs2017/fftw3
#win32:FFTW3FLIB = libfftw3f-3
#win32:FFTW3FLIB_RELEASE = $${FFTW3FLIB}
#win32:FFTW3FLIB_DEBUG = $${FFTW3FLIB}
#win32:FFTW3FLIBDIR = $${FFTW3FROOT}
#win32:FFTW3FLIBDIR_RELEASE = $${FFTW3FLIBDIR}
#win32:FFTW3FLIBDIR_DEBUG = $${FFTW3FLIBDIR}
#win32:FFTW3FINCLUDEDIR = $${FFTW3FROOT}

win32:BOOSTROOT = c:/api-vs2017/boost-1.69.0
#win32:BOOSTINCLUDEDIR = $${BOOSTROOT}

win32:ASIOROOT = c:/api-vs2017/asio

win32:SNDFILEROOT = c:/api-vs2017/sndfile

win32:IOWKITROOT = c:/api-vs2017/iowkit_1_5

win32:PROTOBUFROOT = c:/api-vs2017/protobuf-cpp-3.6.1
#win32:PROTOBUFLIB_RELEASE = libprotobuf-lite
#win32:PROTOBUFLIB_DEBUG = libprotobuf-lited
#win32:PROTOBUFLIBDIR = $${PROTOBUFROOT}/lib
#win32:PROTOBUFINCLUDEDIR = $${PROTOBUFROOT}/include
#win32:PROTOBUFPROTOC = $${PROTOBUFROOT}/bin/protoc.exe

win32:PROTOBUFPYTHONROOT = c:/api-vs2017/protobuf-python-2.5.0
#win32:PROTOBUFPYTHONDIR = $${PROTOBUFPYTHONROOT}/python-lib
#win32:PROTOBUFPYTHONPROTOC = $${PROTOBUFPYTHONROOT}/protoc.exe

win32:NIC3ROOT = c:/api-vs2017/nic3
#win32:NIC3BINARIESDIR = $$NIC3ROOT/binaries
#win32:NIC3PYTHONDIR = $$NIC3ROOT/python

win32:NIC4ROOT = c:/api-vs2017/nic4
#win32:NIC4BINDIR = $${NIC4ROOT}/binaries
#win32:NIC4LIBDIR = $${NIC4ROOT}/lib/c/Win32
#win32:NIC4INCLUDEDIR = $${NIC4ROOT}/c/include

win32:DOTROOT = c:/api-vs2017/dot
#win32:DOTBINDIR = $${DOTROOT}/bin
#win32:DOTBINEXECUTABLE = $${DOTBIN}/dot.exe

win32:LIBLOROOT = c:/api-vs2017/liblo-0.29
#win32:LIBLOLIB = lo
#win32:LIBLOLIBDIR_RELEASE = $${LIBLOROOT}/release/lib
#win32:LIBLOLIBDIR_DEBUG = $${LIBLOROOT}/debug/lib
#win32:LIBLOINCLUDEDIR = $${LIBLOROOT}/include

win32:OPENSSLROOT = c:/api-vs2017/openssl-1.0.2q
#win32:OPENSSLBINDIR = $${OPENSSLROOT}/bin
#win32:OPENSSLLIBDIR = $${OPENSSLROOT}/lib
#win32:LIBSSLLIB = ssleay32
#win32:LIBCRYPTOLIB = libeay32

win32:LIBSSH2ROOT = c:/api-vs2017/libssh2-1.8.0
#win32:LIBSSH2BINDIR = $${LIBSSH2ROOT}/bin
#win32:LIBSSH2LIBDIR = $${LIBSSH2ROOT}/lib
#win32:LIBSSH2LIB = libssh2

win32:LIBGIT2ROOT = c:/api-vs2017/libgit2
#win32:LIBGIT2BINDIR = $${LIBGIT2ROOT}/bin
#win32:LIBGIT2LIBDIR = $${LIBGIT2ROOT}/lib
#win32:LIBGIT2LIB = git2

#win32:LSLROOT = c:/api-vs2017/lsl
#win32:LSLINCLUDEDIR = $${LSLROOT}
#win32:LSLLIBDIR = $${LSLROOT}

# Installation paths for Unix
#unix:PREFIXDIR = /usr/local
#unix:CONFDIR = /etc
#unix:DATADIR = $$PREFIXDIR/share/apex
#unix:APPDIR = $$PREFIXDIR/share/applications
#unix:ICONDIR = $$PREFIXDIR/share/icons/hicolor
#unix:BINDIR = $$PREFIXDIR/bin
#unix:LIBDIR = $$PREFIXDIR/lib
#unix:PLUGINDIR = $$PREFIXDIR/lib/apex

# Installation paths for Windows
#win32:PREFIXDIR = $${DESTDIR}_installed
#win32:CONFDIR = $$PREFIXDIR/config
#win32:DATADIR = $$PREFIXDIR
#win32:ICONDIR = $$PREFIXDIR/icons
#win32:BINDIR = $$PREFIXDIR/bin
#win32:LIBDIR = $$PREFIXDIR/bin
#win32:PLUGINDIR = $$PREFIXDIR/plugins

# Streamapp
#CONFIG += MAC         #use OsX config

DEFINES += WIN32
