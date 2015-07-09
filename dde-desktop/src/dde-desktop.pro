#-------------------------------------------------
#
# Project created by QtCreator 2015-06-24T09:14:17
#
#-------------------------------------------------

QT       += core gui dbus

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = dde-desktop
TEMPLATE = app

include(./widgets/widgets.pri)

# build dir
BuildDir =build_$$QT_VERSION

CONFIG(debug, debug|release) {
    DESTDIR = $$PWD/$$BuildDir/debug
    OBJECTS_DIR = $$PWD/$$BuildDir/debug/.obj
    MOC_DIR = $$PWD/$$BuildDir/debug/.moc
    RCC_DIR = $$PWD/$$BuildDir/debug/.rcc
    UI_DIR = $$PWD/$$BuildDir/debug/.ui
} else {
    DESTDIR = $$PWD/$$BuildDir/release
    OBJECTS_DIR = $$PWD/$$BuildDir/release/.obj
    MOC_DIR = $$PWD/$$BuildDir/release/.moc
    RCC_DIR = $$PWD/$$BuildDir/release/.rcc
    UI_DIR = $$PWD/$$BuildDir/release/.ui
}

CONFIG+=c++11


SOURCES += main.cpp \
    views/desktopframe.cpp \
    views/griditem.cpp \
    views/translucentframe.cpp \
    views/desktopitem.cpp \
    views/gridmanager.cpp \
    views/signalmanager.cpp \
    controllers/menucontroller.cpp \
    controllers/appcontroller.cpp \
    app/desktopapp.cpp \
    views/desktopitemmanager.cpp \
    controllers/dbuscontroller.cpp \
    dbusinterface/filelistjob_interface.cpp \
    dbusinterface/fileoperations_interface.cpp \
    dbusinterface/fileoperationsflags_interface.cpp \
    dbusinterface/trashmonitor_interface.cpp \
    dbusinterface/dbustype.cpp \
    dbusinterface/filemonitorInstance_interface.cpp \
    dbusinterface/monitormanager_interface.cpp \
    views/keyeventmanager.cpp

HEADERS  += \
    views/desktopframe.h \
    views/griditem.h \
    views/translucentframe.h \
    views/desktopitem.h \
    views/gridmanager.h \
    views/signalmanager.h \
    views/global.h \
    controllers/menucontroller.h \
    controllers/appcontroller.h \
    app/desktopapp.h \
    views/desktopitemmanager.h \
    dbusinterface/trashmonitor_interface.h \
    controllers/dbuscontroller.h \
    dbusinterface/fileoperationsflags_interface.h \
    dbusinterface/fileoperations_interface.h \
    dbusinterface/filelistjob_interface.h \
    dbusinterface/dbustype.h \
    dbusinterface/filemonitorInstance_interface.h \
    dbusinterface/monitormanager_interface.h \
    views/keyeventmanager.h



RESOURCES += \
    skin.qrc

DISTFILES += \
    dbusinterface/filelistjob.xml \
    dbusinterface/filemonitor.xml \
    dbusinterface/fileoperations.xml \
    dbusinterface/fileoperationsflags.xml \
    dbusinterface/trashmonitor.xml
