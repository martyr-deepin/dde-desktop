/**
 * Copyright (C) 2015 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#ifndef DEFINE_H
#define DEFINE_H

#include "daemon.h"
#include "widgets/singleton.h"
#include "widgets/commandlinemanager.h"
#include "views/global.h"

#include <DLog>

#include <QDBusInterface>
#include <QDBusConnection>
#include <QDBusPendingCall>


#undef signals
extern "C" {
#include <gtk/gtk.h>
}
#define signals public

static void requrestUpdateIcons()
{
    GtkIconTheme *gs = gtk_icon_theme_get_default();
    auto a = gtk_icon_theme_get_example_icon_name(gs);
    if (a != NULL) { g_free(a); }
    //can not passing QObject to the callback function,so use signal
    emit signalManager->gtkIconThemeChanged();
}

void initGtkThemeWatcher()
{
    GtkIconTheme *gs = gtk_icon_theme_get_default();
    g_signal_connect(gs, "changed",
                     G_CALLBACK(requrestUpdateIcons), NULL);
    auto a = gtk_icon_theme_get_example_icon_name(gs);
    if (a != NULL) { g_free(a); }
}

void debug_daemon_off()
{
#if defined(QT_NO_DEBUG)
    daemonize();
#endif
}

void RegisterLogger()
{
    bool isSet =  CommandLineManager::instance()->isSet("logDestination");
    QString value = CommandLineManager::instance()->value("logDestination");
    if (isSet) {
        if (value == "stdout") {
            DTK_UTIL_NAMESPACE::DLogManager::registerConsoleAppender();
        } else if (value == "file") {
            DTK_UTIL_NAMESPACE::DLogManager::registerFileAppender();
        } else {
            DTK_UTIL_NAMESPACE::DLogManager::registerFileAppender();
        }
    } else {
#if !defined(QT_NO_DEBUG)
        DTK_UTIL_NAMESPACE::DLogManager::registerConsoleAppender();
#endif
        DTK_UTIL_NAMESPACE::DLogManager::registerFileAppender();
    }
}

// let startdde know that we've already started.
void RegisterDdeSession()
{
    char envName[] = "DDE_SESSION_PROCESS_COOKIE_ID";

    QByteArray cookie = qgetenv(envName);
    qunsetenv(envName);

    if (!cookie.isEmpty()) {
        QDBusInterface iface("com.deepin.SessionManager",
                             "/com/deepin/SessionManager",
                             "com.deepin.SessionManager",
                             QDBusConnection::sessionBus());
        iface.asyncCall("Register", QString(cookie));
    }
}

#endif // DEFINE_H

