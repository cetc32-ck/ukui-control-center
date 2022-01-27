#-------------------------------------------------
#
# Project created by QtCreator 2019-02-28T14:09:42
#
#-------------------------------------------------

include(../../../env.pri)
include($$PROJECT_COMPONENTSOURCE/switchbutton.pri)
include($$PROJECT_COMPONENTSOURCE/hoverwidget.pri)
include($$PROJECT_COMPONENTSOURCE/imageutil.pri)

QT       += widgets
QT       += core gui widgets dbus

TEMPLATE = lib
CONFIG += plugin

TARGET = $$qtLibraryTarget(pen)
DESTDIR = ../..
target.path = $${PLUGIN_INSTALL_DIRS}

INCLUDEPATH += \
               $$PROJECT_COMPONENTSOURCE \
               $$PROJECT_ROOTDIR \

#DEFINES += QT_DEPRECATED_WARNINGS

SOURCES += \
        pen.cpp

HEADERS += \
        pen.h

FORMS += \
        pen.ui

INSTALLS += target