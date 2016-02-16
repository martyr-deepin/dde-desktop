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
#include "dialogs/cleartrashdialog.h"

#include "dbusinterface/services/desktopadaptor.h"
#include "widgets/commandlinemanager.h"
#include "app/dbusworker.h"
#include <iostream>

#include <QMetaType>
#include <QApplication>
#include <QTranslator>



int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

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

        int reslut = app.exec();
        qDebug() << "exits " << app.applicationName() << reslut;
    }else{
        qDebug() << "dde-desktop is running!";
    }
}
