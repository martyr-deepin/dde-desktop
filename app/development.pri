target.path = /usr/bin/

#desktop_files.path = /usr/share/applications/
#desktop_files.files = data/applications/*

#qm_files.path= /usr/share/dde-desktop/translations/
#qm_files.files = translations/*.qm

#services.path = /usr/share/dbus-1/services
#services.files = dbusservices/com.deepin.dde.desktop.service

#INSTALLS += target qm_files services desktop_files
INSTALLS += target

GitVersion = $$system(git rev-parse HEAD)
DEFINES += GIT_VERSION=\\\"$$GitVersion\\\"

