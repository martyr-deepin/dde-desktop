#-------------------------------------------------
#
# Project created by QtCreator 2015-07-24T19:22:07
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = dde-luancher
TEMPLATE = app


SOURCES += main.cpp \
    views/launcherframe.cpp \
    views/util.cpp \
    views/navigationbar.cpp \
    views/basetablewidget.cpp \
    views/apptablewidget.cpp \
    views/sectiontablewidget.cpp \
    views/signalmanager.cpp \
    views/global.cpp \
    views/categoryframe.cpp

HEADERS += \
    views/launcherframe.h \
    views/util.h \
    views/navigationbar.h \
    views/basetablewidget.h \
    views/apptablewidget.h \
    views/sectiontablewidget.h \
    views/signalmanager.h \
    views/global.h \
    views/categoryframe.h

RESOURCES += \
    skin.qrc

