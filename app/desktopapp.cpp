#include "desktopapp.h"
#include "views/global.h"
#include "views/desktopbox.h"
#include "views/desktopitemmanager.h"
#include "views/desktopframe.h"
#include "controllers/appcontroller.h"
#include "dialogs/dtaskdialog.h"
#include "dialogs/confirmdeletedialog.h"
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

    connect(signalManager, SIGNAL(moveJobAdded(QMap<QString,QString>)),
            m_taskDialog, SLOT(addCopyMoveTask(QMap<QString,QString>)));
    connect(signalManager, SIGNAL(moveJobRemoved(QMap<QString,QString>)),
            m_taskDialog, SLOT(removeTaskWidget(QMap<QString,QString>)));
    connect(signalManager, SIGNAL(moveJobDataUpdated(QMap<QString,QString>,QMap<QString,QString>)),
            m_taskDialog, SLOT(handleUpdateTaskWidget(QMap<QString,QString>,QMap<QString,QString>)));
    connect(m_taskDialog, SIGNAL(abortMoveTask(QMap<QString,QString>)),
            signalManager, SIGNAL(abortMoveTask(QMap<QString,QString>)));

    connect(signalManager, SIGNAL(deleteFilesExcuted(QStringList)),
            this, SLOT(confimDelete(QStringList)));
    connect(signalManager, SIGNAL(deleteJobAdded(QMap<QString,QString>)),
            m_taskDialog, SLOT(addCopyMoveTask(QMap<QString,QString>)));
    connect(signalManager, SIGNAL(deleteJobRemoved(QMap<QString,QString>)),
            m_taskDialog, SLOT(removeTaskWidget(QMap<QString,QString>)));
    connect(signalManager, SIGNAL(deleteJobDataUpdated(QMap<QString,QString>,QMap<QString,QString>)),
            m_taskDialog, SLOT(handleUpdateTaskWidget(QMap<QString,QString>,QMap<QString,QString>)));
    connect(m_taskDialog, SIGNAL(abortDeleteTask(QMap<QString,QString>)),
            signalManager, SIGNAL(abortDeleteTask(QMap<QString,QString>)));

    connect(m_taskDialog, SIGNAL(conflictRepsonseConfirmed(QMap<QString,QString>,QMap<QString,QVariant>)),
            this, SLOT(confimConflict(QMap<QString,QString>,QMap<QString,QVariant>)));

    connect(m_taskDialog, SIGNAL(conflictShowed(QMap<QString,QString>)),
            signalManager, SIGNAL(conflictTimerStoped(QMap<QString,QString>)));
    connect(m_taskDialog, SIGNAL(conflictHided(QMap<QString,QString>)),
            signalManager, SIGNAL(conflictTimerReStarted(QMap<QString,QString>)));

    connect(signalManager, SIGNAL(conflictDialogShowed(QMap<QString,QString>)),
            m_taskDialog, SLOT(showConflictDiloagByJob(QMap<QString,QString>)));


    connect(qApp, SIGNAL(aboutToQuit()), this, SIGNAL(closed()));
}

void DesktopApp::confimDelete(const QStringList &files){
    m_deletefiles = files;
    m_deletefiles.removeOne(ComputerUrl);
    m_deletefiles.removeOne(TrashUrl);

    emit signalManager->unCheckedItem(ComputerUrl);
    emit signalManager->unCheckedItem(TrashUrl);

    if (m_deletefiles.length() == 0){
        return;
    }

    ConfirmDeleteDialog d;
    QString message;
    if (files.length() == 1){
        message = tr("Are you sure to delete %1 ?").arg(QFileInfo(m_deletefiles.at(0)).fileName());
    }else{
        message = tr("Are you sure to delete these %1 items?").arg(QString::number(m_deletefiles.length()));
    }
    d.setMessage(message);
    connect(&d, SIGNAL(buttonClicked(int)), this, SLOT(handleDeleteAction(int)));
    int code = d.exec();
    qDebug() << code << "dialog close";
}

void DesktopApp::handleDeleteAction(int index){
    switch (index) {
    case 0:
        return;
        break;
    case 1:
        emit signalManager->filesDeleted(m_deletefiles);
        break;
    default:
        break;
    }
}

void DesktopApp::confimConflict(const QMap<QString, QString> &jobDetail, const QMap<QString, QVariant> &response){
    qDebug() << jobDetail << response;
    emit signalManager->conflictRepsonseConfirmed(jobDetail, response);
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

