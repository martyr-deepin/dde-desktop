#-------------------------------------------------
#
# Project created by QtCreator 2015-06-24T09:14:17
#
#-------------------------------------------------

QT       += core gui widgets svg dbus x11extras

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = dde-desktop
TEMPLATE = app

include(./widgets/widgets.pri)
include(./cutelogger/cutelogger.pri)
include(./dialogs/dialogs.pri)
include(./background/background.pri)


PKGCONFIG += x11 gtk+-2.0 xcb xcb-ewmh gsettings-qt dui
CONFIG += c++11 link_pkgconfig
#DEFINES += QT_NO_DEBUG_OUTPUT
DEFINES += QT_MESSAGELOGCONTEXT

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
    app/xcb_misc.cpp \
    app/daemon.cpp \
    filemonitor/filemonitorwoker.cpp \
    filemonitor/filemonitor.cpp \
    dbusinterface/renamejob_interface.cpp \
    controllers/renamejobcontroller.cpp \
    dbusinterface/emptytrashjob_interface.cpp \
    controllers/deletejobcontroller.cpp \
    dbusinterface/deletejob_interface.cpp \
    dbusinterface/menu_interface.cpp \
    dbusinterface/dbusdocksetting.cpp \
    controllers/fileconflictcontroller.cpp \
    dbusinterface/services/conflictdaptor.cpp \
    dbusinterface/services/desktopadaptor.cpp \
    app/dbusworker.cpp \
    dbusinterface/displayinterface.cpp \
    dbusinterface/appearancedaemon_interface.cpp \
    controllers/copyjobworker.cpp \
    controllers/movejobworker.cpp \
    controllers/deletejobworker.cpp \
    dbusinterface/pinyin_interface.cpp \
    dbusinterface/dbusclientmanager.cpp


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
    app/xcb_misc.h \
    app/daemon.h \
    app/define.h \
    filemonitor/filemonitorwoker.h \
    filemonitor/filemonitor.h \
    dbusinterface/renamejob_interface.h \
    controllers/renamejobcontroller.h \
    dbusinterface/emptytrashjob_interface.h \
    controllers/deletejobcontroller.h \
    dbusinterface/deletejob_interface.h \
    dbusinterface/menu_interface.h \
    dbusinterface/dbusdocksetting.h \
    controllers/fileconflictcontroller.h \
    dbusinterface/services/conflictdaptor.h \
    dbusinterface/services/desktopadaptor.h \
    app/dbusworker.h \
    dbusinterface/displayinterface.h \
    dbusinterface/appearancedaemon_interface.h \
    controllers/copyjobworker.h \
    controllers/movejobworker.h \
    controllers/deletejobworker.h \
    dbusinterface/pinyin_interface.h \
    dbusinterface/dbusclientmanager.h

# Automating generation .qm files from .ts files
system($$PWD/translate_generation.sh)


RESOURCES += \
    skin.qrc \
    dialogs.qrc

DISTFILES += \
    dbusinterface/filelistjob.xml \
    dbusinterface/filemonitor.xml \
    dbusinterface/fileoperations.xml \
    dbusinterface/fileoperationsflags.xml \
    dbusinterface/trashmonitor.xml \
    dbusinterface/emptytrashjob.xml

target.path = /usr/bin/

qm_files.files = translations/*.qm
qm_files.path= /usr/share/dde-desktop/translations/

services.path = /usr/share/dbus-1/services
services.files = dbusservices/com.deepin.dde.desktop.service

INSTALLS += target qm_files services

OTHER_FILES += \
    dbusinterface/pinyin.xml
