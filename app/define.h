#ifndef DEFINE_H
#define DEFINE_H

#include "logmanager.h"
#include "daemon.h"
#include "widgets/singleton.h"
#include "widgets/commandlinemanager.h"
#include "views/global.h"

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
    GtkIconTheme* gs = gtk_icon_theme_get_default();
    auto a = gtk_icon_theme_get_example_icon_name(gs);
    if (a != NULL) g_free(a);
    //can not passing QObject to the callback function,so use signal
    emit signalManager->gtkIconThemeChanged();
}

void initGtkThemeWatcher()
{
    GtkIconTheme* gs = gtk_icon_theme_get_default();
    g_signal_connect(gs, "changed",
                     G_CALLBACK(requrestUpdateIcons), NULL);
    auto a = gtk_icon_theme_get_example_icon_name(gs);
    if (a != NULL) g_free(a);
}

void debug_daemon_off(){
    #if defined(QT_NO_DEBUG)
    daemonize();
    #endif
}

void RegisterLogger(){
    bool isSet =  CommandLineManager::instance()->isSet("logDestination");
    QString value = CommandLineManager::instance()->value("logDestination");
    if (isSet){
        if (value == "stdout"){
            Singleton<LogManager>::instance()->initConsoleAppender();
        }else if (value == "file"){
            Singleton<LogManager>::instance()->initRollingFileAppender();
        }else{
            Singleton<LogManager>::instance()->initRollingFileAppender();
        }
    }else{
#if !defined(QT_NO_DEBUG)
        Singleton<LogManager>::instance()->initConsoleAppender();
#endif
        Singleton<LogManager>::instance()->initRollingFileAppender();
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

