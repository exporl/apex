################################################################
#                                                              #
#   Qmake project for streamapp.                               #
#   Creates a makefile for building a static library           #
#   Expects the following environment variables to be defined: #
#     - NSPDIR the Intel Nsp directory                         #
#     - QWTDIR the Qwt directory                               #
#     - FREETYPEDIR the FreeType2 directory                    #
#                                                              #
################################################################

BASEDIR = ../../..
CLEBS += +portaudio
win32:CLEBS *= +asio
unix:CLEBS *= +jack
include($${BASEDIR}/clebs.pri)

# set main config
debug:TARGET = streamappd
release:TARGET = streamapp
TEMPLATE    = lib
VERSION     = 2.0
INCLUDEPATH += . _fromv3
CONFIG      += staticlib
#warn_on exceptions stl

win32 {
#  DEFINES -= UNICODE        # Qt defines this by default for VC8
  INCLUDEPATH += _archs/win32
}

unix {

  INCLUDEPATH += _archs/linux

  !MAC {
    DEFINES += LINUX
  } else {
    DEFINES += MAC
    INCLUDEPATH += _archs/osx
  }

  ARM {
    DEFINES += DAVINCI_ARM
    QMAKE_CXXFLAGS += -march=armv5t -fno-strict-aliasing
  }

    release:DEFINES += NDEBUG
}

!NOUSEQT {
  # TODO: necessary?
  USEQTGUI {
    DEFINES += USESQT
  }
}

UICORE {
  message( "Using UiCore" )

  ARM {
    TARMG = $$(ARMUIROOT)
    isEmpty( TARMG ) {
      error( "ARMUIROOT not defined" )
    } else {
      INCLUDEPATH += $(ARMUIROOT)
    }
  }
  !isEmpty( TFREETYPEDIR ) {
    INCLUDEPATH += $${TFREETYPEDIR}
  } else {
    error( "FREETYPEDIR not defined" )
  }
}

!NOUSEQT {
  HEADERS +=  appcore/qt_inifile.h \
              appcore/qt_utils.h \
              appcore/qt_msghandler.h

  USEQTGUI {
    HEADERS +=  guicore/colors.h \
                guicore/customlayout.h \
                guicore/customlayoutwidget.h \
                guicore/fileselector.h \
                guicore/labeledit.h \
                guicore/minidumpgui.h \
                guicore/msgwindow.h \
                guicore/simpledialogbox.h \
                guicore/slideredit.h \
                guicore/userinput.h \
                guicore/userinputbase.h \
                guicore/widgetattributes.h

    USEQWT {
      HEADERS +=  guicore/qwtplot.h \
                  guicore/qwtpartialcurve.h \
                  guicore/scrollbar.h \
                  guicore/scrollzoomer.h
    }
  }
}

SOURCES +=  audioformatconvertor.cpp \
            audioformatstream.cpp \
            audiosamplebuffer.cpp \
            bufferedreader.cpp \
            bufferedreaderthread.cpp \
            bufferedprocessor.cpp \
            compare.cpp \
            connections.cpp \
            eofcheck.cpp \
            memorystream.cpp \
            multiproc.cpp \
            multistream.cpp \
            silentreader.cpp \
            streamappfactory.cpp \
            appcore/events/eventrouter.cpp \
            appcore/messages/message.cpp \
            appcore/messages/messagehandler.cpp \
            appcore/messages/messagemanager.cpp \
            appcore/threads/thread.cpp \
            appcore/threads/readwritelock.cpp \
            appcore/threads/slicedthread.cpp \
            callback/manualcallbackrunner.cpp \
            callback/multicallback.cpp \
            callback/streamappcallbacks.cpp \
            containers/rawmemory.cpp \
            containers/rawmemorybundle.cpp \
            examples/latencytester.cpp \
            examples/rmstester.cpp \
            examples/playandrecord.cpp \
            examples/wav2bin.cpp \
            file/binaryfile.cpp \
            file/multiwavefile.cpp \
            file/wavefile.cpp \
            generators/sinegenerator.cpp \
            processors/buffer.cpp \
            processors/matrixmixer.cpp \
            processors/spectrum.cpp \
            processors/gate.cpp \
            resamplers/resamplingbuffer.cpp \
            utils/stdredirector.cpp \
            utils/systemutils.cpp \
            utils/tracer.cpp \
            utils/tracermemory.cpp

win32 {
  SOURCES +=  _archs/win32/win32_threads.cpp \
              _archs/win32/win32_cputicker.cpp \
              _archs/win32/win32_ctimer1.cpp \
              _archs/win32/win32_minidump.cpp \
              _archs/win32/win32_misc.cpp \
              _fromv3/_archs/win32/automation/clsid.cpp \
              #_fromv3/_archs/win32/automation/dispatchinfo.cpp \
              #_fromv3/_archs/win32/automation/idispatch.cpp \
              #_fromv3/_archs/win32/automation/olecontroller.cpp \
             # _fromv3/_archs/win32/automation/olecontrollerhlp.cpp \
              #_fromv3/core/text/wchars.cpp
}

unix {
  SOURCES +=  _archs/linux/linux_threads.cpp
  !ARM {
    SOURCES += _archs/linux/linux_misc.cpp
  }
}

!NOSOUNDCARD {
  SOURCES += soundcard/bufferdropcheck.cpp
  HEADERS += soundcard/bufferdropcheck.h

  !NOPORTAUDIO {
    SOURCES +=  soundcard/portaudiowrapper.cpp
    HEADERS +=  soundcard/portaudiowrapper.h
  }

  !unix:!NOASIO {
    SOURCES +=  _archs/win32/win32_asiowrapper.cpp
#                $${ASIOROOT}/common/asio.cpp \
 #               $${ASIOROOT}/host/asiodrivers.cpp
#    win32 {
#      SOURCES += $${ASIOROOT}/host/pc/asiolist.cpp#
#    }
#    MAC {
#      SOURCES += $${ASIOROOT}/host/mac/asioshlib.cpp \
# SOURCES +=                $${ASIOROOT}/host/mac/codefragments.cpp
#    }
  }

  unix:!NOJACK {
    SOURCES += _archs/linux/linux_jackwrapper.cpp
  }

  unix:COREAUDIO {
    SOURCES += _archs/osx/osx_coreaudiowrapper.cpp
  }

}

!NOMIXER {
  SOURCES +=  mixers/mixer.cpp

  win32 {
    SOURCES +=  _archs/win32/win32_mmemixer.cpp \
                _archs/win32/win32_mmemixerimpl.cpp
  }

  !NORME {
     SOURCES +=  mixers/rmemixer.cpp

    win32 {
      SOURCES += _archs/win32/win32_rmemixer.cpp
    }

    unix {
      SOURCES += _archs/linux/linux_rmemixer.cpp
    }
  }
}

win32:NSP {
  SOURCES +=  _archs/win32/win32_nspfirfilter.cpp \
              _archs/win32/win32_nspresampler.cpp
}

!NOUSEQT {
  SOURCES +=  appcore/qt_inifile.cpp \
              appcore/qt_msghandler.cpp

  USEQTGUI {
    SOURCES +=  guicore/customlayout.cpp \
                guicore/fileselector.cpp \
                guicore/labeledit.cpp \
                guicore/minidumpgui.cpp \
                guicore/msgwindow.cpp \
                guicore/simpledialogbox.cpp \
                guicore/slideredit.cpp \
                guicore/userinput.cpp \
                guicore/userinputbase.cpp \
                guicore/widgetattributes.cpp

    USEQWT {
      SOURCES +=  guicore/qwtplot.cpp \
                  guicore/qwtpartialcurve.cpp \
                  guicore/scrollbar.cpp \
                  guicore/scrollzoomer.cpp
    }
  }
}

NETWORK {
  SOURCES +=  network/socketfunc.cpp \
              network/tcpsocket.cpp \
              network/eventsocketconn.cpp \
              network/eventsocketconnlogger.cpp \
              network/eventsocketconnmgr.cpp \
              network/socketconn.cpp \
              network/socketconnserver.cpp

  win32 {
    SOURCES +=  _archs/win32/win32_getmacaddress.cpp
  }
}

UICORE {
  SOURCES +=   uicore/component.cpp \
              uicore/screen.cpp \
              uicore/menu.cpp \
              uicore/menuloop.cpp \
              uicore/button.cpp \
              uicore/painter.cpp \
              uicore/gfx.cpp \
              uicore/font.cpp

  ARM {
    SOURCES +=  _archs/arm_le/arm_le_misc.cpp \
                _archs/arm_le/arm_le_uicore.cpp \
                _archs/arm_le/arm_le_framebuffer.cpp \
                _archs/linux/linux_framebuffer_shared.cpp
  }
  else {
    USEQTGUI {
      SOURCES +=  uicore/qt_framebufferemu.cpp \
                  controller/qt_buttoncontrolemu.cpp
    }
  }
}

CONTROLLER {
  ARM {
    SOURCES += _archs/arm_le/arm_le_msp430.cpp
  }
}
