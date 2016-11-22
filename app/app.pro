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
    desktop.cpp \
    pluginloader.cpp \
    view/canvasviewhelper.cpp \
#    view/canvasview.cpp \
    model/dfileselectionmodel.cpp \
    view/canvasgridview.cpp \
    view/private/canvasviewprivate.cpp \
    presenter/positionmanager.cpp \
    presenter/canvasgridpresenter.cpp

# Automating generation .qm files from .ts files
#system($$PWD/translate_generation.sh)

include($$PWD/util/util.pri)

HEADERS += \
    view/screenframe.h \
    config/config.h \
    desktop.h \
    pluginloader.h \
    view/canvasviewhelper.h \
#    view/canvasview.h \
    model/dfileselectionmodel.h \
    view/canvasgridview.h \
    view/private/canvasviewprivate.h \
    presenter/positionmanager.h \
    global/coorinate.h \
    global/singleton.h \
    presenter/canvasgridpresenter.h

RESOURCES += \
    resource/theme/theme.qrc
