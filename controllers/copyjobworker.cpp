#include "copyjobworker.h"
#include "views/global.h"
#include "dbuscontroller.h"
#include "dbusinterface/copyjob_interface.h"
#include "dbusinterface/fileoperations_interface.h"
#include "dbusinterface/services/conflictdaptor.h"
#include "controllers/fileconflictcontroller.h"

CopyjobWorker::CopyjobWorker(QStringList files, QString destination, QObject *parent) :
    QObject(parent),
    m_files(files),
    m_destination(destination)
{
    m_conflictController = new FileConflictController;
    m_progressTimer = new QTimer;
    m_progressTimer->setInterval(1000);
    m_time = new QTime;
    initConnect();
}

void CopyjobWorker::initConnect(){
    connect(this, SIGNAL(startJob()), this, SLOT(start()));
    connect(this, SIGNAL(finished()), this, SLOT(handleFinished()));
    connect(m_progressTimer, SIGNAL(timeout()), this, SLOT(handleTimeout()));
    connect(signalManager, SIGNAL(abortCopyTask(QMap<QString,QString>)),
            this, SLOT(handleTaskAborted(QMap<QString,QString>)));
}

QStringList CopyjobWorker::getFiles(){
    return m_files;
}

QString CopyjobWorker::getDestination(){
    return m_destination;
}

QString CopyjobWorker::getJobPath(){
    return m_copyjobPath;
}

void CopyjobWorker::start(){
    copyFiles(m_files, m_destination);
}

void CopyjobWorker::copyFiles(QStringList files, QString destination){

    if (files.length() == 0)
        return;

    QDBusPendingReply<QString, QDBusObjectPath, QString> reply = \
            dbusController->getFileOperationsInterface()->NewCopyJob(
                files,
                desktopLocation,
                "",
                0,
                ConflictAdaptor::staticServerPath(),
                m_conflictController->getObjectPath(),
                ConflictAdaptor::staticInterfaceName()
                );

    reply.waitForFinished();
    if (!reply.isError()){
        QString service = reply.argumentAt(0).toString();
        QString path = qdbus_cast<QDBusObjectPath>(reply.argumentAt(1)).path();
        qDebug() << "copy files" << files << path;
        m_copyjobPath = path;
        m_jobDetail.insert("jobPath", path);
        m_jobDetail.insert("type", "copy");
        m_conflictController->setJobDetail(m_jobDetail);
        m_jobDataDetail.insert("destination",  QFileInfo(decodeUrl(desktopLocation)).fileName());
        m_copyJobInterface = new CopyJobInterface(service, path, QDBusConnection::sessionBus(), this);
        connectCopyJobSignal();
        m_copyJobInterface->Execute();
        m_progressTimer->start();
        m_time->start();
        emit signalManager->copyJobAdded(m_jobDetail);
    }else{
        qCritical() << reply.error().message();
        m_progressTimer->stop();
    }
}


void CopyjobWorker::connectCopyJobSignal(){
    if (m_copyJobInterface){
        connect(m_copyJobInterface, SIGNAL(Done(QString)), this, SLOT(copyJobExcuteFinished(QString)));
        connect(m_copyJobInterface, SIGNAL(Aborted()), this, SLOT(copyJobAbortFinished()));
        connect(m_copyJobInterface, SIGNAL(Copying(QString)), this, SLOT(onCopyingFile(QString)));
        connect(m_copyJobInterface, SIGNAL(TotalAmount(qlonglong,ushort)),
                this, SLOT(setTotalAmount(qlonglong,ushort)));
        connect(m_copyJobInterface, SIGNAL(ProcessedAmount(qlonglong,ushort)),
                this, SLOT(onCopyingProcessAmount(qlonglong,ushort)));
    }
}


void CopyjobWorker::disconnectCopyJobSignal(){
    if (m_copyJobInterface){
        disconnect(m_copyJobInterface, SIGNAL(Done(QString)), this, SLOT(copyJobExcuteFinished(QString)));
        disconnect(m_copyJobInterface, SIGNAL(Aborted()), this, SLOT(copyJobAbortFinished()));
        disconnect(m_copyJobInterface, SIGNAL(Copying(QString)), this, SLOT(onCopyingFile(QString)));
        disconnect(m_copyJobInterface, SIGNAL(TotalAmount(qlonglong,ushort)),
                this, SLOT(setTotalAmount(qlonglong,ushort)));
        disconnect(m_copyJobInterface, SIGNAL(ProcessedAmount(qlonglong,ushort)),
                this, SLOT(onCopyingProcessAmount(qlonglong,ushort)));
    }
}


void CopyjobWorker::copyJobExcuteFinished(QString file){
    disconnectCopyJobSignal();
    m_copyJobInterface->deleteLater();
    m_copyJobInterface = NULL;
    m_progressTimer->stop();
    emit finished();
    qDebug() << "copy job finished" << file;
}

void CopyjobWorker::copyJobAbort(){
    if (m_copyJobInterface){
        m_progressTimer->stop();
        m_copyJobInterface->Abort();
    }
}

void CopyjobWorker::copyJobAbortFinished(){
    qDebug() << "copy job aborted";
    copyJobExcuteFinished("");
}

void CopyjobWorker::onCopyingFile(QString file){
    emit signalManager->copyingFileChaned(file);
    m_jobDataDetail.insert("file", QFileInfo(decodeUrl(file)).fileName());
    qDebug() << "onCopyingFile" << file;
    if (m_jobDetail.contains("jobPath")){
        emit signalManager->copyJobDataUpdated(m_jobDetail, m_jobDataDetail);
    }
}

void CopyjobWorker::setTotalAmount(qlonglong amount, ushort type){
    qDebug() << "========="<< amount << type;
    m_totalAmout = amount;
}

void CopyjobWorker::onCopyingProcessAmount(qlonglong progress, ushort type){
//    qDebug() << "onCopyingProcessAmount" << this << progress << type;
    m_currentProgress = progress;
}

void CopyjobWorker::handleTimeout(){
    float speed = (m_currentProgress - m_lastProgress) / (1024 * 1024);
//    qDebug() << speed;
    m_lastProgress = m_currentProgress;
    int remainTime = (m_totalAmout - m_currentProgress) / speed;
//    qDebug() << remainTime;
    m_jobDataDetail.insert("speed", QString::number(speed));
    m_jobDataDetail.insert("remainTime", QString::number(remainTime));
    m_jobDataDetail.insert("progress", "10");
    emit signalManager->copyJobDataUpdated(m_jobDetail, m_jobDataDetail);
}

void CopyjobWorker::handleFinished(){
    if (m_jobDetail.contains("jobPath")){
        emit signalManager->copyJobRemoved(m_jobDetail);
    }
    m_conflictController->unRegisterDBusService();
}

void CopyjobWorker::handleTaskAborted(const QMap<QString, QString> &jobDetail){
    if (jobDetail == m_jobDetail){
        copyJobAbort();
    }
}
