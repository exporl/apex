# Template for a custom configuration
# You should not need this if you have all dependencies in the right
# directories or you are using Debian GNU/Linux.
#
# The only variable that is defined before is BASEDIR

# Uncomment for release version
#RELEASE = 1

# Uncomment if nmake has trouble with long command lines
# CLEBS *= nosilent

# Uncomment to override make files that want precompiled headers
#CLEBS -= pch

# Uncomment to disable certain modules although all dependencies are available
# Do not disable flash here!
CLEBS_DISABLED += src/plugins/matlabfilter src/plugins/ledfeedbackplugin src/plugins/osccontroller src/programs/ledcontrollertest examples src/plugins/spinplugin

# Use multiple cores in windows
#win32:QMAKE_CXXFLAGS += /MP2


# Dependencies for Unix:
#unix:MATLABROOT=/usr/local/matlab
#unix:IOWKITROOT=/usr/local

# Dependencies for Windows

#win32:PORTAUDIOROOT = $$BASEDIR/../api/portaudio
#win32:PORTAUDIOLIB = portaudio_x86
#win32:PORTAUDIOLIB_RELEASE = $${PORTAUDIOLIB}
#win32:PORTAUDIOLIB_DEBUG = $${PORTAUDIOLIB}
#win32:PORTAUDIOLIBDIR = $${PORTAUDIOROOT}/lib
#win32:PORTAUDIOLIBDIR_RELEASE = $${PORTAUDIOLIBDIR}/release
#win32:PORTAUDIOLIBDIR_DEBUG = $${PORTAUDIOLIBDIR}/debug
#win32:PORTAUDIOINCLUDEDIR = $${PORTAUDIOROOT}/include

#win32:NUCLEUSROOT = $$BASEDIR/../api/nucleus
#win32:NUCLEUSLIBDIR = $${NUCLEUSROOT}/libraries/msvc/v7.1
#win32:NUCLEUSINCLUDEDIR = $${NUCLEUSROOT}/include

#win32:FFTW3ROOT = $$BASEDIR/../api/fftw3
#win32:FFTW3LIB = libfftw3-3
#win32:FFTW3LIB_RELEASE = $${FFTW3LIB}
#win32:FFTW3LIB_DEBUG = $${FFTW3LIB}
#win32:FFTW3LIBDIR = $${FFTW3ROOT}
#win32:FFTW3LIBDIR_RELEASE = $${FFTW3LIBDIR}
#win32:FFTW3LIBDIR_DEBUG = $${FFTW3LIBDIR}
#win32:FFTW3INCLUDEDIR = $${FFTW3ROOT}

#win32:FFTW3FROOT = $$BASEDIR/../api/fftw3
#win32:FFTW3FLIB = libfftw3f-3
#win32:FFTW3FLIB_RELEASE = $${FFTW3FLIB}
#win32:FFTW3FLIB_DEBUG = $${FFTW3FLIB}
#win32:FFTW3FLIBDIR = $${FFTW3FROOT}
#win32:FFTW3FLIBDIR_RELEASE = $${FFTW3FLIBDIR}
#win32:FFTW3FLIBDIR_DEBUG = $${FFTW3FLIBDIR}
#win32:FFTW3FINCLUDEDIR = $${FFTW3FROOT}

#win32:BOOSTROOT = $$BASEDIR/../api/boost
#win32:BOOSTINCLUDEDIR = $${BOOSTROOT}

#win32:ASIOROOT = $$BASEDIR/../api/asio
#win32:SNDFILEROOT = $$BASEDIR/../api/sndfile
#win32:IOWKITROOT= $$BASEDIR/../api/iowkit

#win32:PROTOBUFROOT = $$BASEDIR/../api/protobuf-cpp-3.6.1
#win32:PROTOBUFLIB_RELEASE = libprotobuf-lite
#win32:PROTOBUFLIB_DEBUG = libprotobuf-lited
#win32:PROTOBUFLIBDIR = $${PROTOBUFROOT}/lib
#win32:PROTOBUFINCLUDEDIR = $${PROTOBUFROOT}/include
#win32:PROTOBUFPROTOC = $${PROTOBUFROOT}/bin/protoc.exe

#win32:PROTOBUFPYTHONROOT = $$BASEDIR/../api/protobuf-python-2.5.0
#win32:PROTOBUFPYTHONDIR = $${PROTOBUFPYTHONROOT}/python-lib
#win32:PROTOBUFPYTHONPROTOC = $${PROTOBUFPYTHONROOT}/protoc.exe

#win32:NIC3ROOT = $$BASEDIR/../api/nic3
#win32:NIC3BINARIESDIR = $$NIC3ROOT/binaries
#win32:NIC3PYTHONDIR = $$NIC3ROOT/python

#win32:DOTROOT = $$BASEDIR/../api/dot
#win32:DOTBINDIR = $${DOTROOT}/bin
#win32:DOTBINEXECUTABLE = $${DOTBIN}/dot.exe

#win32:LIBLOROOT = $$BASEDIR/../api/liblo
#win32:LIBLOLIB = lo
#win32:LIBLOLIBDIR_RELEASE = $${LIBLOROOT}/release/lib
#win32:LIBLOLIBDIR_DEBUG = $${LIBLOROOT}/debug/lib
#win32:LIBLOINCLUDEDIR = $${LIBLOROOT}/include

# Installation paths for Unix
#unix:PREFIX = $$BASEDIR/bin/installed
#unix:CONFDIR = /etc
#unix:DATADIR = $$PREFIX/share/apex
#unix:APPDIR = $$PREFIX/share/applications
#unix:ICONDIR = $$PREFIX/share/icons/hicolor
#unix:BINDIR = $$PREFIX/bin
#unix:LIBDIR = $$PREFIX/lib
#unix:PLUGINDIR = $$PREFIX/lib/apex

# Installation paths for Windows
#win32:PREFIX = $${DESTDIR}_installed
#win32:CONFDIR = $$PREFIX/config
#win32:DATADIR = $$PREFIX
#win32:ICONDIR = $$PREFIX/icons
#win32:BINDIR = $$PREFIX/bin
#win32:LIBDIR = $$PREFIX/bin
#win32:PLUGINDIR = $$PREFIX/plugins

# Streamapp
#CONFIG += MAC         #use OsX config
