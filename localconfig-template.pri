# Template for a custom configuration
# You should not need this if you have all dependencies in the right
# directories or you are using Debian GNU/Linux.
#
# The only variable that is defined before is BASEDIR

# Uncomment for release version
#RELEASE = 1

# Uncomment to override make files that want precompiled headers
#CLEBS -= pch

# Uncomment to disable certain modules although all dependencies are available
# Do not disable flash or pa5 here!
#CLEBS_DISABLED = syllib original tester hrtf sink screeneditor perl clarion

# Uncomment for support of the programmable attenuator
#CONFIG += PA5

# Use multiple cores in windows
#win32:QMAKE_CXXFLAGS += /MP2


# Dependencies for Unix:
#unix:MATLABROOT=/usr/local/matlab
#unix:IOWKITROOT=/usr/local

# Dependencies for Windows
#win32:XALANCROOT = $$BASEDIR/../api/xalan
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

#win32:XERCESCROOT = $$BASEDIR/../api/xerces
#win32:XERCESCLIB = xerces-c_2
#win32:XERCESCLIB_RELEASE = $${XERCESCLIB}
#win32:XERCESCLIB_DEBUG = $${XERCESCLIB}d
#win32:XERCESCLIBDIR = $${XERCESCROOT}/build/win32/vc7.1
#win32:XERCESCLIBDIR_RELEASE = $${XERCESCLIBDIR}/release
#win32:XERCESCLIBDIR_DEBUG = $${XERCESCLIBDIR}/debug
#win32:XERCESCDLL = xerces-c_2_7
#win32:XERCESCDLL_RELEASE = $${XERCESCDLL}.dll
#win32:XERCESCDLL_DEBUG = $${XERCESCDLL}d.dll
#win32:XERCESCDLLDIR = $${XERCESCLIBDIR}
#win32:XERCESCDLLDIR_RELEASE = $${XERCESCDLLDIR}/release
#win32:XERCESCDLLDIR_DEBUG = $${XERCESCDLLDIR}/debug
#win32:XERCESCINCLUDEDIR = $${XERCESCROOT}/src

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

# Boost
#isEmpty(BOOSTROOT):BOOSTROOT = $$BASEDIR/../api/boost
#isEmpty(BOOSTINCLUDEDIR):BOOSTINCLUDEDIR = $${BOOSTROOT}/include/boost-1_37
#isEmpty(BOOSTLIBDIR):BOOSTLIBDIR = $${BOOSTROOT}/lib
#isEmpty(BOOSTLIBDIR_RELEASE):BOOSTLIBDIR_RELEASE = $${BOOSTLIBDIR}
#isEmpty(BOOSTLIBDIR_DEBUG):BOOSTLIBDIR_DEBUG = $${BOOSTLIBDIR}
#isEmpty(BOOSTPOLIB_RELEASE):BOOSTPOLIB_RELEASE = boost_program_options-vc90-mt-1_37
#isEmpty(BOOSTPOLIB_DEBUG):BOOSTPOLIB_DEBUG = boost_program_options-vc90-mt-gd-1_37


#win32:ASIOROOT = $$BASEDIR/../api/asio
#win32:SNDFILEROOT = $$BASEDIR/../api/sndfile
#win32:IOWKITROOT= $$BASEDIR/../api/iowkit

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
