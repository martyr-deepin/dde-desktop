#include "app/desktopapp.h"
#include "widgets/singleton.h"
#include "widgets/themeappicon.h"
#include "app/define.h"
#include "views/global.h"
#include <QApplication>
#include "dialogs/cleartrashdialog.h"

int main(int argc, char *argv[])
{
    debug_daemon_off();
    QApplication a(argc, argv);
    debug_log_console_on();
    DesktopApp desktop;
    desktop.show();
    Singleton<ThemeAppIcon>::instance()->gtkInit();
    dbusController->init();
    RegisterDdeSession();
    LOG_INFO() << "Starting the application";
    int reslut = a.exec();
    LOG_INFO() << "exits " << a.applicationName() << reslut;
}
