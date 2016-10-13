#-------------------------------------------------
#
# Project created by QtCreator 2015-06-24T09:14:17
#
#-------------------------------------------------

include($$PWD/../build.pri)
include($$PWD/../plugin/interface/interface.pri)
include($$PWD/development.pri)

QT       += core gui widgets svg dbus x11extras
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TEMPLATE    = app
TARGET      = dde-desktop
DESTDIR     = $$BUILD_DIST
CONFIG      += c++11 link_pkgconfig
PKGCONFIG   += x11 gtk+-2.0 xcb xcb-ewmh gsettings-qt gio-2.0 \
    dtkbase dtkutil dtkwidget dde-file-manager

SOURCES += \
    main.cpp \
    view/screenframe.cpp \
    config/config.cpp \
    view/widgetcanvas.cpp \
    model/widgetmodel.cpp \
    presenter/widgetpresenter.cpp \
    desktop.cpp \
    pluginloader.cpp

# Automating generation .qm files from .ts files
#system($$PWD/translate_generation.sh)

include($$PWD/util/util.pri)

HEADERS += \
    view/screenframe.h \
    config/config.h \
    view/widgetcanvas.h \
    model/widgetmodel.h \
    presenter/widgetpresenter.h \
    desktop.h \
    pluginloader.h

RESOURCES += \
    resource/theme/theme.qrc
