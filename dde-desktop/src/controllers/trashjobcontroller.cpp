#include "trashjobcontroller.h"
#include "views/global.h"
#include "dbuscontroller.h"

TrashJobController::TrashJobController(QObject *parent) : QObject(parent)
{
    m_trashMonitorInterface = new TrashMonitorInterface(FileMonitor_service, TrashMonitor_path, QDBusConnection::sessionBus(), this);
    monitorTrash();
    initConnect();
}

void TrashJobController::initConnect(){
    connect(signalManager, SIGNAL(trashEmptyChanged(bool)), this, SLOT(setTrashEmptyFlag(bool)));
    connect(signalManager, SIGNAL(trashingAboutToExcute(QStringList)),
            this, SLOT(trashJobExcute(QStringList)));
    connect(signalManager, SIGNAL(trashingAboutToAborted()),
            this, SLOT(trashJobAbort()));
}

void TrashJobController::monitorTrash(){
    QDBusPendingReply<uint> reply = m_trashMonitorInterface->ItemCount();
    reply.waitForFinished();
    if (!reply.isError()){
        uint count = reply.argumentAt(0).toUInt();
        if (count == 0){
            m_isTrashEmpty = true;
        }else{
            m_isTrashEmpty = false;
        }
    }else{
        qDebug() << reply.error().message();
    }
    connect(m_trashMonitorInterface, SIGNAL(ItemCountChanged(uint)), this, SLOT(updateTrashIconByCount(uint)));
}


void TrashJobController::setTrashEmptyFlag(bool flag){
    m_isTrashEmpty = flag;
}

void TrashJobController::trashJobExcute(QStringList files){
    QDBusPendingReply<QString, QDBusObjectPath, QString> reply = DBusController::instance()->getFileOperationsInterface()->NewTrashJob(files, false, "", "", "");
    reply.waitForFinished();
    if (!reply.isError()){
        QString service = reply.argumentAt(0).toString();
        QString path = qdbus_cast<QDBusObjectPath>(reply.argumentAt(1)).path();
        m_trashJobInterface = new TrashJobInterface(service, path, QDBusConnection::sessionBus(), this);
        connectTrashSignal();
        m_trashJobInterface->Execute();
    }else{
        qDebug() << reply.error().message();
    }
}

void TrashJobController::connectTrashSignal(){
    if (m_trashJobInterface){
        connect(m_trashJobInterface, SIGNAL(Done()), this, SLOT(deleteFilesFinished()));
        connect(m_trashJobInterface, SIGNAL(Aborted()), this, SLOT(trashJobAbortFinished()));
        connect(m_trashJobInterface, SIGNAL(Trashing(QString)), this, SLOT(onTrashingFile(QString)));
        connect(m_trashJobInterface, SIGNAL(Deleting(QString)), this, SLOT(onDeletingFile(QString)));
        connect(m_trashJobInterface, SIGNAL(ProcessedAmount(qlonglong,ushort)), this, SLOT(onProcessAmount(qlonglong,ushort)));
    }
}

void TrashJobController::disconnectTrashSignal(){
    if (m_trashJobInterface){
        disconnect(m_trashJobInterface, SIGNAL(Done()), this, SLOT(trashJobAbortFinished()));
        disconnect(m_trashJobInterface, SIGNAL(Aborted()), this, SLOT(trashJobAbortFinished()));
        disconnect(m_trashJobInterface, SIGNAL(Trashing(QString)), this, SLOT(onTrashingFile(QString)));
        disconnect(m_trashJobInterface, SIGNAL(Deleting(QString)), this, SLOT(onDeletingFile(QString)));
        disconnect(m_trashJobInterface, SIGNAL(ProcessedAmount(qlonglong,ushort)), this, SLOT(onProcessAmount(qlonglong,ushort)));
    }
}

void TrashJobController::trashJobExcuteFinished(){
    disconnectTrashSignal();
    m_trashJobInterface = NULL;

    qDebug() << "trash files deleted";
}


void TrashJobController::trashJobAbort(){
    if (m_trashJobInterface){
        m_trashJobInterface->Abort();
    }
}

void TrashJobController::trashJobAbortFinished(){
    disconnectTrashSignal();
    m_trashJobInterface = NULL;
}

void TrashJobController::onTrashingFile(QString file){
    emit signalManager->trashingFileChanged(file);
}

void TrashJobController::onDeletingFile(QString file){
    emit signalManager->deletingFileChanged(file);
}

void TrashJobController::onProcessAmount(qlonglong progress, ushort info){
    emit signalManager->processAmountChanged(progress, info);
}

void TrashJobController::updateTrashIconByCount(uint count){
    if (count == 0){
        if (!m_isTrashEmpty){
            DBusController::instance()->requestIconByUrl(TrashUrl, 48);
        }
        m_isTrashEmpty = true;
    }else{
        if (m_isTrashEmpty){
            DBusController::instance()->requestIconByUrl(TrashUrl, 48);
        }
        m_isTrashEmpty = false;
    }
}

TrashJobController::~TrashJobController()
{

}

