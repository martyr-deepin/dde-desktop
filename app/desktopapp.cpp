#include "desktopapp.h"
#include "views/global.h"
#include "views/desktopbox.h"
#include "views/desktopitemmanager.h"
#include "views/desktopframe.h"
#include "controllers/appcontroller.h"
#include "dialogs/dtaskdialog.h"
#include "dbusinterface/services/desktopadaptor.h"
#include <QDBusConnection>
#include <QStandardPaths>


DesktopApp::DesktopApp(QObject *parent) : QObject(parent)
{
    m_desktopBox = new DesktopBox;
    m_taskDialog = new DTaskDialog;
    initConnect();
    registerDBusService();
}

void DesktopApp::initConnect(){
    connect(signalManager, SIGNAL(gridModeChanged(bool)), this, SLOT(saveGridOn(bool)));
    connect(signalManager, SIGNAL(gridSizeTypeChanged(SizeType)), this, SLOT(saveSizeType(SizeType)));
    connect(signalManager, SIGNAL(sortedModeChanged(QDir::SortFlags)), this, SLOT(saveSortFlag(QDir::SortFlags)));
    connect(signalManager, SIGNAL(copyJobAdded(QMap<QString,QString>)),
            m_taskDialog, SLOT(addCopyMoveTask(QMap<QString,QString>)));
    connect(signalManager, SIGNAL(copyJobRemoved(QMap<QString,QString>)),
            m_taskDialog, SLOT(removeTaskWidget(QMap<QString,QString>)));
    connect(signalManager, SIGNAL(copyJobDataUpdated(QMap<QString,QString>,QMap<QString,QString>)),
            m_taskDialog, SLOT(handleUpdateTaskWidget(QMap<QString,QString>,QMap<QString,QString>)));
    connect(m_taskDialog, SIGNAL(abortCopyTask(QMap<QString,QString>)),
            signalManager, SIGNAL(abortCopyTask(QMap<QString,QString>)));
    connect(qApp, SIGNAL(aboutToQuit()), this, SIGNAL(closed()));
}

void DesktopApp::registerDBusService(){
    new DesktopAdaptor(this);
    QDBusConnection conn = QDBusConnection::sessionBus();
    conn.registerObject(DesktopAdaptor::staticInterfacePath(), this);
}

void DesktopApp::saveGridOn(bool mode){
    QSettings settings;
    settings.beginGroup("Desktop");
    settings.setValue("isGridOn", mode);
    settings.endGroup();
}

void DesktopApp::saveSizeType(SizeType type){
    QSettings settings;
    settings.beginGroup("Desktop");
    settings.setValue("sizeType", type);
    settings.endGroup();
}

void DesktopApp::saveSortFlag(QDir::SortFlags flag){
    QSettings settings;
    settings.beginGroup("Desktop");
    settings.setValue("sortFlag", static_cast<int>(flag));
    settings.endGroup();
}

void DesktopApp::show(){
    m_desktopBox->show();
    emit shown();
}

void DesktopApp::hide(){
    m_desktopBox->hide();
}

void DesktopApp::toggle(){
    if (m_desktopBox->isVisible()){
        hide();
    }else{
        show();
    }
}

void DesktopApp::exit(){
    qApp->quit();
}

void DesktopApp::unRegisterDbusService(){
    QDBusConnection conn = QDBusConnection::sessionBus();
    conn.unregisterObject(DesktopAdaptor::staticInterfacePath());
    conn.unregisterService(DesktopAdaptor::staticServerPath());
}

DesktopApp::~DesktopApp()
{

}

