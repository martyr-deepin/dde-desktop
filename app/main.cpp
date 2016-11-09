/**
 * Copyright (C) 2016 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#include <QDBusConnection>

#include <DLog>
#include <DApplication>

#include "util/dde/ddesession.h"
#include "util/gtk/gtkworkaround.h"

#include "config/config.h"
#include "desktop.h"
#include "view/canvasview.h"

using namespace Dtk::Util;
using namespace Dtk::Widget;

int main(int argc, char *argv[])
{
    DApplication app(argc, argv);
    app.setOrganizationName("deepin");
    app.setApplicationName("dde-desktop");
    app.setApplicationVersion((GIT_VERSION));

    DLogManager::registerConsoleAppender();
    DLogManager::registerFileAppender();

    qDebug() << app.applicationVersion();
    app.loadTranslator();

    QDBusConnection conn = QDBusConnection::sessionBus();

    Config::instance();
    // TODO: Gtk Workaround
    Gtk::Init();
//    app.connect(&Gtk::GtkWorkaround::instance(),
//    &Gtk::GtkWorkaround::gtkIconThemeChange,
//                qApp, [=](const QString&){
//        qApp->exit(100);
//    });

    Desktop::instance().loadPlugins();
    Desktop::instance().loadData();
    Desktop::instance().show();

    // Notify dde-desktop start up
    Dde::Session::RegisterDdeSession();

    return app.exec();
}
