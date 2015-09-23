#include "copyjobcontroller.h"
#include "views/global.h"
#include "dbuscontroller.h"
#include "dbusinterface/copyjob_interface.h"
#include "dbusinterface/fileoperations_interface.h"
#include "dbusinterface/services/conflictdaptor.h"

CopyJobController::CopyJobController(QObject *parent) : QObject(parent)
{
    initConnect();
}


void CopyJobController::initConnect(){
    connect(signalManager, SIGNAL(copyFilesExcuted(QStringList,QString)),
            this, SLOT(copyFiles(QStringList,QString)));
    connect(signalManager, SIGNAL(copyJobAboutToAbort()), this, SLOT(copyJobAbort()));
}

void CopyJobController::copyFiles(QStringList files, QString destination){

    if (files.length() == 0)
        return;

    QDBusPendingReply<QString, QDBusObjectPath, QString> reply = \
            dbusController->getFileOperationsInterface()->NewCopyJob(
                files,
                desktopLocation,
                "",
                0,
                ConflictAdaptor::staticServerPath(),
                ConflictAdaptor::staticInterfacePath(),
                ConflictAdaptor::staticInterfaceName()
                );

    reply.waitForFinished();
    if (!reply.isError()){
        QString service = reply.argumentAt(0).toString();
        QString path = qdbus_cast<QDBusObjectPath>(reply.argumentAt(1)).path();
        qDebug() << "copy files" << files << path;
        m_copyJobInterface = new CopyJobInterface(service, path, QDBusConnection::sessionBus(), this);
        connectCopyJobSignal();
        m_copyJobInterface->Execute();
    }else{
        qCritical() << reply.error().message();
    }
}


void CopyJobController::connectCopyJobSignal(){
    if (m_copyJobInterface){
        connect(m_copyJobInterface, SIGNAL(Done(QString)), this, SLOT(copyJobExcuteFinished(QString)));
        connect(m_copyJobInterface, SIGNAL(Aborted()), this, SLOT(copyJobAbortFinished()));
        connect(m_copyJobInterface, SIGNAL(Copying(QString)), this, SLOT(onCopyingFile(QString)));
        connect(m_copyJobInterface, SIGNAL(ProcessedAmount(qlonglong,ushort)),
                this, SLOT(onCopyingProcessAmount(qlonglong,ushort)));
    }
}


void CopyJobController::disconnectCopyJobSignal(){
    if (m_copyJobInterface){
        disconnect(m_copyJobInterface, SIGNAL(Done(QString)), this, SLOT(copyJobExcuteFinished(QString)));
        disconnect(m_copyJobInterface, SIGNAL(Aborted()), this, SLOT(copyJobAbortFinished()));
        disconnect(m_copyJobInterface, SIGNAL(Copying(QString)), this, SLOT(onCopyingFile(QString)));
        disconnect(m_copyJobInterface, SIGNAL(ProcessedAmount(qlonglong,ushort)),
                this, SLOT(onCopyingProcessAmount(qlonglong,ushort)));
    }
}


void CopyJobController::copyJobExcuteFinished(QString file){
    disconnectCopyJobSignal();
    m_copyJobInterface->deleteLater();
    m_copyJobInterface = NULL;
    qDebug() << "copy job finished" << file;
}

void CopyJobController::copyJobAbort(){
    if (m_copyJobInterface){
        m_copyJobInterface->Abort();
    }
}

void CopyJobController::copyJobAbortFinished(){
    disconnectCopyJobSignal();
    m_copyJobInterface->deleteLater();
    m_copyJobInterface = NULL;
    qDebug() << "copy job aborted";
}


void CopyJobController::onCopyingFile(QString file){
    emit signalManager->copyingFileChaned(file);
    qDebug() << "onCopyingFile" << file;
}

void CopyJobController::onCopyingProcessAmount(qlonglong progress, ushort info){
    qDebug() << "onCopyingProcessAmount" << progress << info;
    emit signalManager->copyingProcessAmountChanged(progress, info);

}


CopyJobController::~CopyJobController()
{

}

