#-------------------------------------------------
#
# Project created by QtCreator 2016-03-30T06:59:21
#
#-------------------------------------------------

QT       += core network

QT       -= gui

DESTDIR = ../bin
OBJECTS_DIR = ../build
MOC_DIR=../build
TARGET = mdachunk
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

INCLUDEPATH += ../../mountainlab/mountainsort/src/mda
DEPENDPATH += ../../mountainlab/mountainsort/src/mda
VPATH += ../../mountainlab/mountainsort/src/mda
SOURCES += main.cpp \
    diskreadmda.cpp \
    diskwritemda.cpp \
    mda.cpp \
    mdaio.cpp \
    mdaclient.cpp \
    remotereadmda.cpp \
    usagetracking.cpp \
    utils/get_command_line_params.cpp \
    utils/textfile.cpp
INCLUDEPATH += utils
HEADERS += \
    diskreadmda.h \
    diskwritemda.h \
    mda.h \
    mdaio.h \
    mdaclient.h \
    usagetracking.h \
    remotereadmda.h \
    utils/get_command_line_params.h \
    utils/textfile.h

INCLUDEPATH += ../../mountainlab/mountainsort/src/utils
DEPENDPATH += ../../mountainlab/mountainsort/src/utils
VPATH += ../../mountainlab/mountainsort/src/utils
HEADERS += msmisc.h
SOURCES += msmisc.cpp


