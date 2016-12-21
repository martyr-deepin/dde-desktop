DEFINES += QT_MESSAGELOGCONTEXT

BUILD_DIST = $$PWD/build/dest

EDITION=COMMUNITY
ARCH = $$QMAKE_HOST.arch
isEqual(ARCH, sw_64) | isEqual(ARCH, mips64) | isEqual(ARCH, mips32) {
    EDITION=RACCOON
}

isEqual(EDITION, RACCOON) {
    DEFINES += DDE_COMPUTER_TRASH
}

DEFINES += DDE_DBUS_DISPLAY

isEqual(EDITION, RACCOON) {
    PKGCONFIG += dtkbase-0-2 dtkutil-0-2 dtkwidget-0-2
} else {
    PKGCONFIG += dtkbase dtkutil dtkwidget
}
