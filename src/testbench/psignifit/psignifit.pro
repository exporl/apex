BASEDIR = ../../..
CLEBS *= psignifit
TARGET = psignifittest
TEMPLATE = app
include($$BASEDIR/clebs.pri)

CONFIG *= qtestlib

SOURCES *= psignifittest.cpp

HEADERS *= psignifittest.h
