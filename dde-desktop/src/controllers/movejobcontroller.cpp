#include "movejobcontroller.h"
#include "dbusinterface/movejob_interface.h"
#include "dbusinterface/fileoperations_interface.h"
#include "views/global.h"

MoveJobController::MoveJobController(QObject *parent) : QObject(parent)
{
    initConnect();
}

void MoveJobController::initConnect(){
    connect(signalManager, SIGNAL(moveFilesExcuted(QStringList,QString)),
            this, SLOT(moveFiles(QStringList,QString)));
    connect(signalManager, SIGNAL(moveJobAboutToAbort()), this, SLOT(moveJobAbort()));
}


void MoveJobController::moveFiles(QStringList files, QString destination){
    LOG_INFO() << files << destination;
    QDBusPendingReply<QString, QDBusObjectPath, QString> reply = DBusController::instance()->getFileOperationsInterface()->NewMoveJob(files, destination, "",  0, "",  "", "");
    reply.waitForFinished();
    if (!reply.isError()){
        QString service = reply.argumentAt(0).toString();
        QString path = qdbus_cast<QDBusObjectPath>(reply.argumentAt(1)).path();
        LOG_INFO() << "move files" << files << path;
        m_moveJobInterface = new MoveJobInterface(service, path, QDBusConnection::sessionBus(), this);
        connectMoveJobSignal();
        m_moveJobInterface->Execute();
    }else{
        LOG_ERROR() << reply.error().message();
    }
}

void MoveJobController::connectMoveJobSignal(){
    if (m_moveJobInterface){
        connect(m_moveJobInterface, SIGNAL(Done()), this, SLOT(moveJobExcuteFinished()));
        connect(m_moveJobInterface, SIGNAL(Aborted()), this, SLOT(moveJobAbortFinished()));
        connect(m_moveJobInterface, SIGNAL(Moving(QString)), this, SLOT(onMovingFile(QString)));
        connect(m_moveJobInterface, SIGNAL(ProcessedAmount(qulonglong,ushort)),
                this, SLOT(onMovingProcessAmount(qulonglong,ushort)));
    }
}


void MoveJobController::disconnectMoveJobSignal(){
    if (m_moveJobInterface){
        disconnect(m_moveJobInterface, SIGNAL(Done()), this, SLOT(moveJobExcuteFinished()));
        disconnect(m_moveJobInterface, SIGNAL(Aborted()), this, SLOT(moveJobAbortFinished()));
        disconnect(m_moveJobInterface, SIGNAL(Moving(QString)), this, SLOT(onMovingFile(QString)));
        disconnect(m_moveJobInterface, SIGNAL(ProcessedAmount(qulonglong,ushort)),
                this, SLOT(onMovingProcessAmount(qulonglong,ushort)));
    }
}


void MoveJobController::moveJobExcuteFinished(){
    disconnectMoveJobSignal();
    m_moveJobInterface = NULL;
    LOG_INFO() << "move job finished";
}

void MoveJobController::moveJobAbort(){
    if (m_moveJobInterface){
        m_moveJobInterface->Abort();
    }
}

void MoveJobController::moveJobAbortFinished(){
    disconnectMoveJobSignal();
    m_moveJobInterface = NULL;
    LOG_INFO() << "move job aborted";
}


void MoveJobController::onMovingFile(QString file){
    emit signalManager->movingFileChaned(file);
}

void MoveJobController::onMovingProcessAmount(qulonglong progress, ushort info){
    emit signalManager->movingProcessAmountChanged(progress, info);
}

MoveJobController::~MoveJobController()
{

}

