#include "app/desktopapp.h"
#include "widgets/singleton.h"
#include "app/define.h"
#include "views/global.h"
#include <QApplication>


int main(int argc, char *argv[])
{
    debug_daemon_off();
    QApplication a(argc, argv);

    DesktopApp desktop;
    desktop.show();
    dbusController->init();
    debug_log_console_on();
    LOG_INFO() << "Starting the application";
    int reslut = a.exec();
    LOG_INFO() << "exits " << a.applicationName() << reslut;
}
