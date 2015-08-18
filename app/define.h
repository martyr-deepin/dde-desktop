#ifndef DEFINE_H
#define DEFINE_H

#include "logmanager.h"
#include "daemon.h"
#include "widgets/singleton.h"
#include <Logger.h>

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

#endif // DEFINE_H

