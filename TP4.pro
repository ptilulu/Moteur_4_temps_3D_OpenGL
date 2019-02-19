#-------------------------------------------------
#
# Project created by QtCreator 2019-01-12T12:27:11
#
#-------------------------------------------------

QT       += core gui

CONFIG += c++14

LIBS += -lGLU32\
        -lOpengl32

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = tp4
TEMPLATE = app


SOURCES += main.cpp \
        princ.cpp   \
        glarea.cpp  \
        demicylindre.cpp

HEADERS  += princ.h \
        glarea.h    \
        demicylindre.h

FORMS    += princ.ui

RESOURCES += \
    tp4.qrc
