isEmpty(PREFIX){
    PREFIX = /usr
}

target.path = $${PREFIX}/bin/

#desktop_files.path = /usr/share/applications/
#desktop_files.files = data/applications/*

#services.path = /usr/share/dbus-1/services
#services.files = dbusservices/com.deepin.dde.desktop.service

translations.path = $${PREFIX}/share/$${TARGET}/translations
translations.files = translations/*.qm

INSTALLS += target translations

GitVersion = $$system(git rev-parse HEAD)
DEFINES += GIT_VERSION=\\\"$$GitVersion\\\"

