/**
 * Copyright (C) 2015 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#include "app/desktopapp.h"
#include "widgets/singleton.h"
#include "widgets/themeappicon.h"
#include "app/define.h"
#include "views/global.h"
#include "xdnd/xdndworkaround.h"
#include "dialogs/cleartrashdialog.h"

#include "dbusinterface/services/desktopadaptor.h"
#include "widgets/commandlinemanager.h"
#include "app/dbusworker.h"
#include <iostream>

#include <QMetaType>
#include <DApplication>
#include <QTranslator>

DWIDGET_USE_NAMESPACE

// Rename app group magic dirs to dirs that's named by their app group name.
static void AppGroupCompatibleWorkaround() {
    QString desktopPath = QStandardPaths::writableLocation(QStandardPaths::DesktopLocation);
    QDir desktopDir(desktopPath);
    QFileInfoList desktopInfoList = desktopDir.entryInfoList(QDir::AllDirs | QDir::Hidden);
    for (QFileInfo info : desktopInfoList) {
        if (isAppGroup(info.filePath())) {
            QString appGroupName = info.filePath().replace(FilePrefix, "").replace(RichDirPrefix, "");
            QFile::rename(info.filePath(), desktopDir.filePath(appGroupName));
        }
    }
}

int main(int argc, char *argv[])
{
    DApplication app(argc, argv);

    QDBusConnection conn = QDBusConnection::sessionBus();

#ifndef QT_DEBUG
    if(conn.registerService(DesktopAdaptor::staticServerPath())){
#else
    if(1){
#endif
        qApp->setOrganizationName("deepin");
        qApp->setApplicationName("dde-desktop");
        qApp->setApplicationVersion("2015-1.0");

        // setup translator
        QTranslator translator;
        translator.load("/usr/share/dde-desktop/translations/dde-desktop_" + QLocale::system().name() + ".qm");
        app.installTranslator(&translator);

        CommandLineManager::instance()->initOptions();
        RegisterLogger();

        qRegisterMetaType<DesktopItemInfoMap>("DesktopItemInfoMap");
        qDBusRegisterMetaType<DesktopItemInfoMap>();

        AppGroupCompatibleWorkaround();

        DesktopApp desktop;
        desktop.show();

        DBusWorker worker;
        QThread dbusThread;
        worker.moveToThread(&dbusThread);
        dbusThread.start();
        emit signalManager->startRequest();

        RegisterDdeSession();

        gtk_init(NULL, NULL);
        gdk_error_trap_push();
        initGtkThemeWatcher();
        XdndWorkaround workaround;
        int reslut = app.exec();
        qDebug() << "exits " << app.applicationName() << reslut;
    }else{
        qDebug() << "dde-desktop is running!";
    }
}
