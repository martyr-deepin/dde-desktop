/**
 * Copyright (C) 2016 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#include <QDebug>
#include <QDBusError>
#include <QDBusConnection>

#include <DLog>
#include <DApplication>

#include "dfmglobal.h"

#include "util/dde/ddesession.h"
#include "util/gtk/gtkworkaround.h"

#include "config/config.h"
#include "desktop.h"

using namespace Dtk::Util;
using namespace Dtk::Widget;

int main(int argc, char *argv[])
{
    DApplication app(argc, argv);
    app.setOrganizationName("deepin");
    app.setApplicationName("dde-desktop");
    app.setApplicationVersion((GIT_VERSION));
    app.setTheme("light");

    DLogManager::registerConsoleAppender();
    DLogManager::registerFileAppender();

    qDebug() << "start "<< app.applicationName() << app.applicationVersion();

    QDBusConnection conn = QDBusConnection::sessionBus();

    if (!conn.registerService(DesktopServiceName)) {
        qDebug() << "registerService Failed, maybe service exist" << conn.lastError();
        exit(0x0002);
    }

    if (!conn.registerObject(DesktopServicePath, Desktop::instance(),
            QDBusConnection::ExportAllSlots |
            QDBusConnection::ExportAllSignals |
            QDBusConnection::ExportAllProperties)) {
        qDebug() << "registerObject Failed" << conn.lastError();
        exit(0x0003);
    }

    app.loadTranslator();

    Config::instance();

    Desktop::instance()->loadData();
    Desktop::instance()->loadView();
    Desktop::instance()->show();

    // Notify dde-desktop start up
    Dde::Session::RegisterDdeSession();

    DFMGlobal::initDialogManager();

    return app.exec();
}
