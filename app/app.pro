#-------------------------------------------------
#
# Project created by QtCreator 2015-06-24T09:14:17
#
#-------------------------------------------------

QT       += core gui widgets svg dbus x11extras
#core-private gui-private
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TEMPLATE    = app
TARGET      = dde-desktop

PKGCONFIG += x11 gtk+-2.0 xcb xcb-ewmh gsettings-qt dtkbase dtkutil dtkwidget gio-2.0 Qt5Xdg
CONFIG += c++11 link_pkgconfig

include($$PWD/development.pri)

SOURCES += \
    main.cpp \
    view/screenframe.cpp

# Automating generation .qm files from .ts files
#system($$PWD/translate_generation.sh)

include($$PWD/util/util.pri)

HEADERS += \
    view/screenframe.h
