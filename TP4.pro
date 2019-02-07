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

TARGET = TP4
TEMPLATE = app


SOURCES += main.cpp\
        princ.cpp \
        glarea.cpp \
        cylindre.cpp

HEADERS  += princ.h \
        glarea.h \
         cylindre.h

FORMS    += princ.ui

RESOURCES += \
    TP4S.qrc
