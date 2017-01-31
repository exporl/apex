BASEDIR = ../..
CLEBS += xalan xerces streamapp apexmain pch apextools apexwriters apexdata

include ($$BASEDIR/clebs.pri)

QT += gui script
QT += qt3support

TARGET = screeneditor

# WTF? TODO FIXME path???
INCLUDEPATH += ../apex
DEPENDPATH += ../apex

SOURCES = \
          answerlabeleditordelegate.cpp \
          arclayouteditordelegate.cpp \
          buttoneditordelegate.cpp \
          editordelegatecreatorvisitor.cpp \
          emptyelementeditordelegate.cpp \
          experimentfilemanager.cpp \
          fileprefixconvertordummy.cpp \
          flashfilesdb.cpp \
          flashplayereditordelegate.cpp \
          functions.cpp \
          gridlayouteditordelegate.cpp \
          imagefilesdb.cpp \
          labeleditordelegatebase.cpp \
          labeleditordelegate.cpp \
          layouteditordelegate.cpp \
          main.cpp \
          parameterlisteditordelegate.cpp \
          pictureeditordelegate.cpp \
          picturelabeleditordelegate.cpp \
          propertieseditor.cpp \
          screeneditoraddwidgetaction.cpp \
          screeneditor.cpp \
          screeneditorexperimentfileparser.cpp \
          screenelementcreator.cpp \
          screenelementeditordelegate.cpp \
          screenwidget.cpp \
          screenwidgetdb.cpp \
          textediteditordelegate.cpp \

HEADERS = \
          answerlabeleditordelegate.h \
          arclayouteditordelegate.h \
          buttoneditordelegate.h \
          emptyelementeditordelegate.h \
          experimentfilemanager.h \
          flashfilesdb.h \
          flashplayereditordelegate.h \
          functions.h \
          gridlayouteditordelegate.h \
          imagefilesdb.h \
          labeleditordelegatebase.h \
          labeleditordelegate.h \
          layouteditordelegate.h \
          parameterlisteditordelegate.h \
          pictureeditordelegate.h \
          picturelabeleditordelegate.h \
          propertieseditor.h \
          screeneditoraddwidgetaction.h \
          screeneditorexperimentfileparser.h \
          screeneditor.h \
          screenelementcreator.h \
          screenelementeditordelegate.h \
          screenwidgetdb.h \
          screenwidget.h \
          textediteditordelegate.h \
