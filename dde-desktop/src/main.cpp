#include "app/desktopapp.h"
#include "app/logmanager.h"
#include "app/daemondesktop.h"
#include <Logger.h>
#include <QApplication>

int main(int argc, char *argv[])
{
    daemonize();
    QApplication a(argc, argv);
    DesktopApp* w = new DesktopApp;
    w->show();
    LogManager::instance()->initConsoleAppender();
    LogManager::instance()->initRollingFileAppender();
    LOG_INFO() << "Starting the application";
    int reslut = a.exec();
    LOG_INFO() << "exits " << a.applicationName() << reslut;

}
