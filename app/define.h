#ifndef DEFINE_H
#define DEFINE_H

#include "logmanager.h"
#include "daemon.h"
#include "widgets/singleton.h"
#include <Logger.h>

#include <QDBusInterface>
#include <QDBusConnection>
#include <QDBusPendingCall>

void debug_daemon_off(){
    #if defined(QT_NO_DEBUG)
    daemonize();
    #endif
}

void debug_log_console_on(){
    #if !defined(QT_NO_DEBUG)
    Singleton<LogManager>::instance()->initConsoleAppender();
    #endif
    Singleton<LogManager>::instance()->initRollingFileAppender();
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

