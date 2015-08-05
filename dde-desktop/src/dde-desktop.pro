#-------------------------------------------------
#
# Project created by QtCreator 2015-06-24T09:14:17
#
#-------------------------------------------------

QT       += core gui dbus x11extras

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = dde-desktop
TEMPLATE = app

include(./widgets/widgets.pri)
include(./cutelogger/cutelogger.pri)

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

PKGCONFIG += x11 xcb xcb-ewmh
CONFIG += c++11 link_pkgconfig


SOURCES += main.cpp \
    views/desktopframe.cpp \
    views/griditem.cpp \
    views/translucentframe.cpp \
    views/desktopitem.cpp \
    views/gridmanager.cpp \
    controllers/menucontroller.cpp \
    controllers/appcontroller.cpp \
    app/desktopapp.cpp \
    views/desktopitemmanager.cpp \
    controllers/dbuscontroller.cpp \
    dbusinterface/trashmonitor_interface.cpp \
    dbusinterface/dbustype.cpp \
    dbusinterface/filemonitorInstance_interface.cpp \
    dbusinterface/monitormanager_interface.cpp \
    views/keyeventmanager.cpp\
    dbusinterface/menumanager_interface.cpp \
    dbusinterface/showmenu_interface.cpp \
    dbusinterface/desktopdaemon_interface.cpp \
    dbusinterface/fileInfo_interface.cpp \
    dbusinterface/fileoperations_interface.cpp \
    dbusinterface/createdirjob_interface.cpp \
    dbusinterface/createfilejob_interface.cpp \
    dbusinterface/createfilefromtemplatejob_interface.cpp \
    views/global.cpp \
    views/appgroupbox.cpp \
    views/apptablewidget.cpp \
    views/appgroupiconframe.cpp \
    dbusinterface/trashjob_interface.cpp \
    dbusinterface/watcherinstance_interface.cpp \
    dbusinterface/clipboard_interface.cpp \
    dbusinterface/movejob_interface.cpp \
    controllers/movejobcontroller.cpp \
    controllers/trashjobcontroller.cpp \
    controllers/copyjobcontroller.cpp \
    dbusinterface/copyjob_interface.cpp \
    views/desktopbox.cpp \
    views/dragdropeventmanager.cpp \
    app/logmanager.cpp \
    app/xcb_misc.cpp \
    app/daemon.cpp \
    filemonitor/filemonitorwoker.cpp \
    filemonitor/filemonitor.cpp


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
    dbusinterface/dbustype.h \
    dbusinterface/filemonitorInstance_interface.h \
    dbusinterface/monitormanager_interface.h \
    views/keyeventmanager.h\
    dbusinterface/menumanager_interface.h \
    dbusinterface/showmenu_interface.h \
    dbusinterface/desktopdaemon_interface.h \
    dbusinterface/fileInfo_interface.h \
    dbusinterface/fileoperations_interface.h \
    dbusinterface/createdirjob_interface.h \
    dbusinterface/createfilejob_interface.h \
    dbusinterface/createfilefromtemplatejob_interface.h \
    views/appgroupbox.h \
    views/apptablewidget.h \
    views/appgroupiconframe.h \
    dbusinterface/trashjob_interface.h \
    dbusinterface/watcherinstance_interface.h \
    dbusinterface/clipboard_interface.h \
    dbusinterface/movejob_interface.h \
    controllers/movejobcontroller.h \
    controllers/trashjobcontroller.h \
    controllers/copyjobcontroller.h \
    dbusinterface/copyjob_interface.h \
    views/desktopbox.h \
    views/dragdropeventmanager.h \
    views/desktopfwd.h \
    views/desktopenums.h \
    app/logmanager.h \
    app/xcb_misc.h \
    app/daemon.h \
    app/define.h \
    filemonitor/filemonitorwoker.h \
    filemonitor/filemonitor.h


RESOURCES += \
    skin.qrc

DISTFILES += \
    dbusinterface/filelistjob.xml \
    dbusinterface/filemonitor.xml \
    dbusinterface/fileoperations.xml \
    dbusinterface/fileoperationsflags.xml \
    dbusinterface/trashmonitor.xml \

target.path = /home/djf/bin
INSTALLS += target
