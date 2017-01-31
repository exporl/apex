# Template for a custom configuration
# You should not need this if you have all dependencies in the right
# directories or you are using Debian GNU/Linux.
#
# The only variable that is defined before is BASEDIR

# Uncomment for release version
#RELEASE = 1

# Uncomment to override make files that want precompiled headers
#CLEBS -= pch

# Some echo command lines are too long
CLEBS *= nosilent

# Uncomment to disable certain modules although all dependencies are available
# Do not disable flash or pa5 here!
# CLEBS_DISABLED = src/plugins/slm_2250 src/tests/programs/activeqt-bk2250

# Uncomment for support of the programmable attenuator
#CONFIG += PA5

# Dependencies for Unix:
MATLABROOT=c:/api-vs2008/matlab71

# Dependencies for Windows
win32:XALANCROOT = c:/api-vs2008/xml-xalan/c
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

win32:XERCESCROOT = c:/api-vs2008/xerces-c-src_2_8_0
#win32:XERCESCLIB = xerces-c_2
#win32:XERCESCLIB_RELEASE = $${XERCESCLIB}
#win32:XERCESCLIB_DEBUG = $${XERCESCLIB}d
win32:XERCESCLIBDIR = $${XERCESCROOT}/build/win32/vc8
#win32:XERCESCLIBDIR_RELEASE = $${XERCESCLIBDIR}/release
#win32:XERCESCLIBDIR_DEBUG = $${XERCESCLIBDIR}/debug
win32:XERCESCDLL = xerces-c_2_8
#win32:XERCESCDLL_RELEASE = $${XERCESCDLL}.dll
#win32:XERCESCDLL_DEBUG = $${XERCESCDLL}d.dll
#win32:XERCESCDLLDIR = $${XERCESCLIBDIR}
#win32:XERCESCDLLDIR_RELEASE = $${XERCESCDLLDIR}/release
#win32:XERCESCDLLDIR_DEBUG = $${XERCESCDLLDIR}/debug
#win32:XERCESCINCLUDEDIR = $${XERCESCROOT}/src

win32:PORTAUDIOROOT = c:/api-vs2008/portaudio
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

win32:NUCLEUSROOT = c:/api-vs2008/nucleus
win32:NUCLEUSLIBDIR = $${NUCLEUSROOT}/libraries/msvc/v7.1
win32:NUCLEUSINCLUDEDIR = $${NUCLEUSROOT}/include

win32:FFTW3ROOT = c:/api-vs2008/fftw3
#win32:FFTW3LIB = libfftw3-3
#win32:FFTW3LIB_RELEASE = $${FFTW3LIB}
#win32:FFTW3LIB_DEBUG = $${FFTW3LIB}
#win32:FFTW3LIBDIR = $${FFTW3ROOT}
#win32:FFTW3LIBDIR_RELEASE = $${FFTW3LIBDIR}
#win32:FFTW3LIBDIR_DEBUG = $${FFTW3LIBDIR}
#win32:FFTW3INCLUDEDIR = $${FFTW3ROOT}

# Boost
win32:BOOSTROOT = c:/api-vs2008/boost
win32:BOOSTINCLUDEDIR = $${BOOSTROOT}/include
#win32:BOOSTLIBDIR = $${BOOSTROOT}/lib
#win32:BOOSTLIBDIR_RELEASE = $${BOOSTLIBDIR}
#win32:BOOSTLIBDIR_DEBUG = $${BOOSTLIBDIR}
win32:BOOSTPOLIB_RELEASE = boost_program_options-vc100-mt-1_47
win32:BOOSTPOLIB_DEBUG = boost_program_options-vc100-mt-gd-1_47

win32:ASIOROOT = c:/api-vs2008/asio

win32:SNDFILEROOT = c:/api-vs2008/sndfile

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
#CONFIG += OPTIMIZE    #for release versions, use a extra optimizations
#CONFIG += ARM         #appends an extra "ARM" to the builddir and lib + auto-dsiables qt etc
#CONFIG += MAC         #use OsX config

#CONFIG += FFTREAL     #use processors/spectrum.h with FFTReal
#CONFIG += NOSOUNDCARD #do not use files in soundcard/
#CONFIG += NOPORTAUDIO #do not use Portaudio
#CONFIG += NOASIO      #do not use Asio (only win32 and mac)
#CONFIG += NOJACK      #do not use Jack audio stuff (unix/mac)
#CONFIG += NOMIXER     #do not use files in mixers/
#CONFIG += NORME       #do not use Rme mixer files in mixers/
#CONFIG += NSP         #use Intel Nsp libraries (only win32)
#CONFIG += NOUSEQT     #do not use Qt (enables qt_ files)
#CONFIG += USEQTGUI    #do not use files in guicore/ except Qwt
#CONFIG += USEQWT      #use Qwt files
#CONFIG += NETWORK     #use files in network/
#CONFIG += UICORE      #use files in uicore/
#CONFIG += CONTROLLER  #use files in controller/
