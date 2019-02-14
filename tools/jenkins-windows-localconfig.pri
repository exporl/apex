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
win32:XALANCROOT = c:/api-vs2010/xml-xalan/c
#win32:XALANCLIB = xalan-c_1
#win32:XALANCLIB_RELEASE = $${XALANCLIB}
#win32:XALANCLIB_DEBUG = $${XALANCLIB}d
#win32:XALANCLIBDIR = $${XALANCROOT}/build/win32/vc7.1
#win32:XALANCLIBDIR_RELEASE = $${XALANCLIBDIR}/release
#win32:XALANCLIBDIR_DEBUG = $${XALANCLIBDIR}/debug
#win32:XALANCDLLS = xalan-c_1_10 xalanmessages_1_10
#win32:for(i,XALANCDLLS):XALANCDLLS_RELEASE *= $${i}.dll
#win32:for(i,XALANCDLLS):XALANCDLLS_DEBUG *= $${i}d.dll
#win32:XALANCDLLDIR = $${XALANCLIBDIR}
#win32:XALANCDLLDIR_RELEASE = $${XALANCDLLDIR}/release
#win32:XALANCDLLDIR_DEBUG = $${XALANCDLLDIR}/debug
#win32:XALANCINCLUDEDIR = $${XALANCROOT}/src

win32:XML2ROOT = c:/api-vs2010/xml2
win32:ICONVROOT = c:/api-vs2010/iconv
win32:ZLIBROOT = c:/api-vs2010/zlib
win32:LSLROOT = c:/api-vs2010/lsl

win32:PORTAUDIOROOT = c:/api-vs2010/portaudio
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

win32:NUCLEUSROOT = c:/api-vs2010/nucleus/c_c++
win32:NUCLEUSLIBDIR = $${NUCLEUSROOT}/libraries/msvc/v7.1
win32:NUCLEUSINCLUDEDIR = $${NUCLEUSROOT}/include

win32:FFTW3ROOT = c:/api-vs2010/fftw3
#win32:FFTW3LIB = libfftw3-3
#win32:FFTW3LIB_RELEASE = $${FFTW3LIB}
#win32:FFTW3LIB_DEBUG = $${FFTW3LIB}
#win32:FFTW3LIBDIR = $${FFTW3ROOT}
#win32:FFTW3LIBDIR_RELEASE = $${FFTW3LIBDIR}
#win32:FFTW3LIBDIR_DEBUG = $${FFTW3LIBDIR}
#win32:FFTW3INCLUDEDIR = $${FFTW3ROOT}

win32:FFTW3FROOT = c:/api-vs2010/fftw3
#win32:FFTW3FLIB = libfftw3f-3
#win32:FFTW3FLIB_RELEASE = $${FFTW3FLIB}
#win32:FFTW3FLIB_DEBUG = $${FFTW3FLIB}
#win32:FFTW3FLIBDIR = $${FFTW3FROOT}
#win32:FFTW3FLIBDIR_RELEASE = $${FFTW3FLIBDIR}
#win32:FFTW3FLIBDIR_DEBUG = $${FFTW3FLIBDIR}
#win32:FFTW3FINCLUDEDIR = $${FFTW3FROOT}

# Boost
win32:BOOSTROOT = c:/api-vs2010/boost
win32:BOOSTINCLUDEDIR = $${BOOSTROOT}/include
#win32:BOOSTLIBDIR = $${BOOSTROOT}/lib
#win32:BOOSTLIBDIR_RELEASE = $${BOOSTLIBDIR}
#win32:BOOSTLIBDIR_DEBUG = $${BOOSTLIBDIR}
win32:BOOSTPOLIB_RELEASE = boost_program_options-vc100-mt-1_47
win32:BOOSTPOLIB_DEBUG = boost_program_options-vc100-mt-gd-1_47

win32:ASIOROOT = c:/api-vs2010/asio

win32:SNDFILEROOT = c:/api-vs2010/sndfile

win32:IOWKITROOT = c:/api-vs2010/iowkit_1_5

win32:PROTOBUFROOT = c:/api-vs2010/protobuf
#win32:PROTOBUFLIB = libprotobuf-lite
#win32:PROTOBUFLIB_RELEASE = $${PROTOBUFLIB}
#win32:PROTOBUFLIB_DEBUG = $${PROTOBUFLIB}
#win32:PROTOBUFLIB_VERSION = -8
#win32:PROTOBUFLIBDIR = $${PROTOBUFROOT}
#win32:PROTOBUFLIBDIR_RELEASE = $${PROTOBUFLIBDIR}/release
#win32:PROTOBUFLIBDIR_DEBUG = $${PROTOBUFLIBDIR}/debug
#win32:PROTOBUFINCLUDEDIR = $${PROTOBUFROOT}/include
#win32:PROTOBUFPROTOC = $${PROTOBUFROOT}/protoc.exe
#win32:PROTOBUFPYTHONDIR = $${PROTOBUFROOT}/python-lib

win32:NIC3ROOT = c:/api-vs2010/nic3
#win32:NIC3BINARIESDIR = $$NIC3ROOT/binaries
#win32:NIC3PYTHONDIR = $$NIC3ROOT/python

win32:DOTROOT = c:/api-vs2010/dot
#win32:DOTBINDIR = $${DOTROOT}/bin
#win32:DOTBINEXECUTABLE = $${DOTBIN}/dot.exe

win32:LIBLOROOT = c:/api-vs2010/liblo
#win32:LIBLOLIB = liblo
#win32:LIBLOLIB_RELEASE = $${LIBLOLIB}
#win32:LIBLOLIB_DEBUG = $${LIBLOLIB}_d
#win32:LIBLOLIBDIR = $${LIBLOROOT}/lib
#win32:LIBLOLIBDIR_RELEASE = $${LIBLOLIBDIR}/ReleaseDLL
#win32:LIBLOLIBDIR_DEBUG = $${LIBLOLIBDIR}/DebugDLL
#win32:LIBLOINCLUDEDIR = $${LIBLOROOT}

win32:OPENSSLROOT = c:/api-vs2010/openssl
#win32:OPENSSLBINDIR = $${OPENSSLROOT}/bin
#win32:OPENSSLLIBDIR = $${OPENSSLROOT}/lib
#win32:LIBSSLLIB = ssleay32
#win32:LIBCRYPTOLIB = libeay32

win32:LIBSSH2ROOT = c:/api-vs2010/libssh2
#win32:LIBSSH2BINDIR = $${LIBSSH2ROOT}/bin
#win32:LIBSSH2LIBDIR = $${LIBSSH2ROOT}/lib
#win32:LIBSSH2LIB = libssh2

win32:LIBGIT2ROOT = c:/api-vs2010/libgit2
#win32:LIBGIT2BINDIR = $${LIBGIT2ROOT}/bin
#win32:LIBGIT2LIBDIR = $${LIBGIT2ROOT}/lib
#win32:LIBGIT2LIB = git2

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
