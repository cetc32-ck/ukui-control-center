#-------------------------------------------------
#
# Project created by QtCreator 2019-02-28T14:09:42
#
#-------------------------------------------------

include(../../../env.pri)
include($$PROJECT_COMPONENTSOURCE/switchbutton.pri)
#include($$PROJECT_COMPONENTSOURCE/hoverwidget.pri)
include($$PROJECT_COMPONENTSOURCE/imageutil.pri)
#include($$PROJECT_COMPONENTSOURCE/switchbutton.pri)
#include($$PROJECT_COMPONENTSOURCE/label.pri)

QT       += widgets
QT       += core gui widgets dbus svg

TEMPLATE = lib
CONFIG += plugin \
          += c++11 \
          link_pkgconfig


PKGCONFIG     += gsettings-qt gio-2.0
LIBS += -L /usr/lib/x86_64-linux-gnur -l KF5BluezQt -lgio-2.0 -lglib-2.0


TARGET = $$qtLibraryTarget(bluetooth)
DESTDIR = ../..
target.path = $${PLUGIN_INSTALL_DIRS}

INCLUDEPATH += \
               $$PROJECT_COMPONENTSOURCE \
               $$PROJECT_ROOTDIR \

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS
#           QT_NO_WARNING_OUTPUT \
#           QT_NO_DEBUG_OUTPUT

#SOURCES += \
#        bluetooth.cpp \
#        bluetooththread.cpp

#HEADERS += \
#        bluetooth.h \
#        bluetooththread.h

SOURCES += \
    bluetooth.cpp \
    bluetoothmain.cpp \
#    bluetoothnamelabel.cpp \
    customizenamelabel.cpp \
    deviceinfoitem.cpp \
    loadinglabel.cpp \
    msgbox.cpp

HEADERS += \
    bluetooth.h \
    bluetoothmain.h \
#    bluetoothnamelabel.h \
    config.h \
    customizenamelabel.h \
    deviceinfoitem.h \
    loadinglabel.h \ \
    msgbox.h

#RESOURCES += \
#    res/img.qrc
#FORMS += \
#        bluetooth.ui

INSTALLS += target
