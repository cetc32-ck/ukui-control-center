include(../../../env.pri)
include($$PROJECT_COMPONENTSOURCE/switchbutton.pri)
include($$PROJECT_COMPONENTSOURCE/label.pri)


QT       +=  core gui widgets network dbus sql

TEMPLATE = lib
CONFIG += plugin link_pkgconfig c++11
PKGCONFIG += gsettings-qt

TARGET = $$qtLibraryTarget(upgrade-icbc)
DESTDIR = ../..
target.path = $${PLUGIN_INSTALL_DIRS}


# 适配窗口管理器圆角阴影
QT += KWindowSystem dbus x11extras

# 适配窗口管理器圆角阴影
LIBS +=-lpthread
LIBS +=-lX11


# 应用图标装载
#icon.path = /usr/share/pixmaps
#icon.files = img/kylin-update-manager.png

# desktop文件装载
desktop.path = /usr/share/applications/
desktop.files = ./src/kylin-system-update.desktop

# gsetting文件装载
#schemes.files = data/org.kylin-update-manager-data.gschema.xml
#schemes.path = /usr/share/glib-2.0/schemas/

config.files = ./config_file/*
config.path = /usr/share/ukui-control-center/upgrade/


INCLUDEPATH   +=  \
                 $$PROJECT_COMPONENTSOURCE \
                 $$PROJECT_ROOTDIR \
# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS \

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    src/deletepkglistwig.cpp \
    src/picturetowhite.cpp \
    src/updatedeleteprompt.cpp \
    src/xatom-helper.cpp \
    src/appupdate.cpp \
    src/backup.cpp \
    src/checkbutton.cpp \
    src/daemonipcdbus.cpp \
    src/historyupdatelistwig.cpp \
    src/m_updatelog.cpp \
    src/mylabel.cpp \
#    src/switchbutton.cpp \
    src/tabwidget.cpp \
    src/traybusthread.cpp \
    src/ukscconn.cpp \
    src/updatedbus.cpp \
    src/updatelog.cpp \
    src/updatesource.cpp \
    src/upgrademain.cpp \
    src/widgetstyle.cpp \
    upgrade.cpp

HEADERS += \
    src/deletepkglistwig.h \
    src/picturetowhite.h \
    src/updatedeleteprompt.h \
    src/xatom-helper.h \
    src/appupdate.h \
    src/backup.h \
    src/checkbutton.h \
    src/connection.h \
    src/daemonipcdbus.h \
    src/historyupdatelistwig.h \
    src/m_updatelog.h \
    src/metatypes.h \
    src/mylabel.h \
    src/shadowwidget.h \
#    src/switchbutton.h \
    src/tabwidget.h \
    src/traybusthread.h \
    src/ukscconn.h \
    src/updatedbus.h \
    src/updatelog.h \
    src/updatesource.h \
    src/upgrademain.h \
    src/utils.h \
    src/widgetstyle.h \
    upgrade.h

# Default rules for deployment.
INSTALLS += target  \
            icon    \
            desktop \
            schemes \
	    config

RESOURCES += \
    shell/res/img.qrc


