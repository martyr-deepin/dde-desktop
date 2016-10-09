target.path = /usr/bin/

desktop_files.files = data/applications/*
desktop_files.path = /usr/share/applications/

qm_files.files = translations/*.qm
qm_files.path= /usr/share/dde-desktop/translations/

services.path = /usr/share/dbus-1/services
services.files = dbusservices/com.deepin.dde.desktop.service

INSTALLS += target qm_files services desktop_files

GitVersion = $$system(git rev-parse HEAD)
DEFINES += GIT_VERSION=\\\"$$GitVersion\\\"


