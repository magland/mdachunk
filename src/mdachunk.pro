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


SOURCES += main.cpp \
    mda/diskreadmda.cpp \
    mda/diskwritemda.cpp \
    mda/mda.cpp \
    mda/mdaio.cpp \
    mda/mdaclient.cpp \
    mda/usagetracking.cpp \
    utils/get_command_line_params.cpp \
    utils/textfile.cpp

INCLUDEPATH += mda utils
HEADERS += \
    mda/diskreadmda.h \
    mda/diskwritemda.h \
    mda/mda.h \
    mda/mdaio.h \
    mda/mdaclient.h \
    mda/usagetracking.h \
    utils/get_command_line_params.h \
    utils/textfile.h
