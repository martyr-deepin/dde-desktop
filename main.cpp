#include "app/desktopapp.h"
#include "widgets/singleton.h"
#include "widgets/themeappicon.h"
#include "app/define.h"
#include "views/global.h"
#include "dialogs/cleartrashdialog.h"
#include "dbusinterface/services/desktopadaptor.h"
#include "widgets/commandlinemanager.h"
#include "app/dbusworker.h"
#include <iostream>

#include <QMetaType>
#include <QApplication>
#include <QTranslator>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QDBusConnection conn = QDBusConnection::sessionBus();
    if(conn.registerService(DesktopAdaptor::staticServerPath())){
        qApp->setOrganizationName("deepin");
        qApp->setApplicationName("dde-desktop");
        qApp->setApplicationVersion("2015-1.0");
        // setup translator
        QTranslator translator;
        translator.load("/usr/share/dde-desktop/translations/dde-desktop_" + QLocale::system().name() + ".qm");
        app.installTranslator(&translator);

        CommandLineManager::instance()->initOptions();
        RegisterLogger();

        qRegisterMetaType<DesktopItemInfoMap>("DesktopItemInfoMap");
        qDBusRegisterMetaType<DesktopItemInfoMap>();

        DesktopApp desktop;
        desktop.show();

        DBusWorker worker;
        QThread dbusThread;
        worker.moveToThread(&dbusThread);
        dbusThread.start();
        emit signalManager->startRequest();

        RegisterDdeSession();
        qDebug() << "Starting the application";

        gtk_init(NULL, NULL);
        initGtkThemeWatcher();

        int reslut = app.exec();
        qDebug() << "exits " << app.applicationName() << reslut;
    }else{
        qDebug() << "dde-desktop is running!";
    }
}
