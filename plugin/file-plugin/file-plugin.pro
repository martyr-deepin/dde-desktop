#-------------------------------------------------
#
# Project created by QtCreator 2016-10-12T10:11:49
#
#-------------------------------------------------

include($$PWD/../../build.pri)
include($$PWD/../interface/interface.pri)

QT       += core gui widgets svg dbus x11extras
#core-private gui-private
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = $$qtLibraryTarget(file-plugin)
DESTDIR = $$BUILD_DIST/plugins
TEMPLATE = lib
CONFIG += c++11 link_pkgconfig

DEFINES += FILEPLUGIN_LIBRARY

SOURCES += fileplugin.cpp \
    filewidget.cpp

HEADERS += fileplugin.h\
        file-plugin_global.h \
    filewidget.h

unix {
    target.path = /usr/lib/dde-desktop/plugin
    INSTALLS += target
}

DISTFILES += \
    dde-desktop-file-plugin.json
