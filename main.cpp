#include "app/desktopapp.h"
#include "widgets/singleton.h"
#include "widgets/themeappicon.h"
#include "app/define.h"
#include "views/global.h"
#include "dialogs/cleartrashdialog.h"
#include "dbusinterface/services/desktopadaptor.h"
#include<iostream>
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QDBusConnection conn = QDBusConnection::sessionBus();
    if(conn.registerService(DesktopAdaptor::staticServerPath())){
        QApplication a(argc, argv);
        debug_log_console_on();
        DesktopApp desktop;
        desktop.show();
        dbusController->loadDesktopItems();
        RegisterDdeSession();
        LOG_INFO() << "Starting the application";
        int reslut = a.exec();
        LOG_INFO() << "exits " << a.applicationName() << reslut;
    }else{
        LOG_INFO() << "dde-desktop is running!";
    }
}
