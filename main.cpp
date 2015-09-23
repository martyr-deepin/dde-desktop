#include "app/desktopapp.h"
#include "widgets/singleton.h"
#include "widgets/themeappicon.h"
#include "app/define.h"
#include "views/global.h"
#include "dialogs/cleartrashdialog.h"
#include "dbusinterface/services/desktopadaptor.h"
#include "widgets/commandlinemanager.h"
#include<iostream>
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    QDBusConnection conn = QDBusConnection::sessionBus();
    if(conn.registerService(DesktopAdaptor::staticServerPath())){
        QApplication app(argc, argv);
        qApp->setOrganizationName("Linux Deepin");
        qApp->setOrganizationDomain("linuxdeepin.org");
        qApp->setApplicationName("dde-desktop");
        qApp->setApplicationVersion("2015-1.0");
        CommandLineManager::instance()->initOptions();
        RegisterLogger();
        DesktopApp desktop;
        desktop.show();
        dbusController->loadDesktopItems();
        RegisterDdeSession();
        qDebug() << "Starting the application";
        int reslut = app.exec();
        qDebug() << "exits " << app.applicationName() << reslut;
    }else{
        qDebug() << "dde-desktop is running!";
    }
}
