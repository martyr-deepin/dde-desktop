#-------------------------------------------------
#
# Project created by QtCreator 2015-06-24T09:14:17
#
#-------------------------------------------------

QT       += core gui

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
    views/gridmanager.cpp

HEADERS  += \
    views/desktopframe.h \
    views/griditem.h \
    views/translucentframe.h \
    views/desktopitem.h \
    views/gridmanager.h



RESOURCES += \
    skin.qrc
