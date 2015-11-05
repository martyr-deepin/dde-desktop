#include "deletejobworker.h"
#include "dbusinterface/deletejob_interface.h"
#include "dbusinterface/fileoperations_interface.h"
#include "dialogs/confirmdeletedialog.h"
#include "views/global.h"

DeletejobWorker::DeletejobWorker(const QStringList &files, QObject *parent):
    m_deletefiles(files),
    QObject(parent)
{
    m_progressTimer = new QTimer;
    m_progressTimer->setInterval(1000);
    m_time = new QTime;
    initConnect();
}

void DeletejobWorker::initConnect(){
    connect(this, SIGNAL(startJob()), this, SLOT(start()));
    connect(this, SIGNAL(finished()), this, SLOT(handleFinished()));
    connect(m_progressTimer, SIGNAL(timeout()), this, SLOT(handleTimeout()));
    connect(signalManager, SIGNAL(abortDeleteTask(QMap<QString,QString>)),
            this, SLOT(handleTaskAborted(QMap<QString,QString>)));
}

void DeletejobWorker::start(){
    deleteFiles(m_deletefiles);
}

void DeletejobWorker::deleteFiles(const QStringList &files){
    qDebug() << "delete" <<files;
    QDBusPendingReply<QString, QDBusObjectPath, QString> reply = dbusController->getFileOperationsInterface()->NewDeleteJob(files, false, "", "", "");
    reply.waitForFinished();
    if (!reply.isError()){
        QString service = reply.argumentAt(0).toString();
        QString path = qdbus_cast<QDBusObjectPath>(reply.argumentAt(1)).path();
        qDebug() << "delete files" << files << path;

        m_deletejobPath = path;
        m_jobDetail.insert("jobPath", path);
        m_jobDetail.insert("type", "delete");

        m_deleteJobInterface = new DeleteJobInterface(service, path, QDBusConnection::sessionBus(), this);
        connectDeleteJobSignal();
        m_deleteJobInterface->Execute();

        m_progressTimer->start();
        m_time->start();
        emit signalManager->deleteJobAdded(m_jobDetail);
    }else{
        qCritical() << reply.error().message();
    }
}

void DeletejobWorker::connectDeleteJobSignal(){
    if (m_deleteJobInterface){
        connect(m_deleteJobInterface, SIGNAL(Done()), this, SLOT(deleteJobExcuteFinished()));
        connect(m_deleteJobInterface, SIGNAL(Aborted()), this, SLOT(deleteJobAbortFinished()));
        connect(m_deleteJobInterface, SIGNAL(Deleting(QString)), this, SLOT(onDeletingFile(QString)));
        connect(m_deleteJobInterface, SIGNAL(TotalAmount(qlonglong,ushort)),
                this, SLOT(setTotalAmount(qlonglong,ushort)));
        connect(m_deleteJobInterface, SIGNAL(ProcessedAmount(qlonglong,ushort)),
                this, SLOT(onDeletingProcessAmount(qlonglong,ushort)));
    }
}


void DeletejobWorker::disconnectDeleteJobSignal(){
    if (m_deleteJobInterface){
        disconnect(m_deleteJobInterface, SIGNAL(Done()), this, SLOT(deleteJobExcuteFinished()));
        disconnect(m_deleteJobInterface, SIGNAL(Aborted()), this, SLOT(deleteJobAbortFinished()));
        disconnect(m_deleteJobInterface, SIGNAL(Deleting(QString)), this, SLOT(onDeletingFile(QString)));
        disconnect(m_deleteJobInterface, SIGNAL(TotalAmount(qlonglong,ushort)),
                this, SLOT(setTotalAmount(qlonglong,ushort)));
        disconnect(m_deleteJobInterface, SIGNAL(ProcessedAmount(qlonglong,ushort)),
                this, SLOT(onDeletingProcessAmount(qlonglong,ushort)));
    }
}


void DeletejobWorker::deleteJobExcuteFinished(){
    disconnectDeleteJobSignal();
    m_deleteJobInterface->deleteLater();
    m_deleteJobInterface = NULL;
    m_deletefiles.clear();
    m_progressTimer->stop();
    emit finished();
    qDebug() << "delete job finished";
}

void DeletejobWorker::deleteJobAbort(){
    if (m_deleteJobInterface){
        m_progressTimer->stop();
        m_deleteJobInterface->Abort();
    }
}

void DeletejobWorker::deleteJobAbortFinished(){
    deleteJobExcuteFinished();
    qDebug() << "delete job aborted";
}


void DeletejobWorker::onDeletingFile(QString file){
    emit signalManager->deletingFileChaned(file);
    qDebug() << "onDeletingFile" << file;

    m_jobDataDetail.insert("file", QFileInfo(decodeUrl(file)).fileName());
    qDebug() << "onCopyingFile" << file;
    if (m_jobDetail.contains("jobPath")){
        emit signalManager->deleteJobDataUpdated(m_jobDetail, m_jobDataDetail);
    }

}

void DeletejobWorker::setTotalAmount(qlonglong amount, ushort type){
    qDebug() << "========="<< amount << type;
    m_totalAmout = amount;
}

void DeletejobWorker::onDeletingProcessAmount(qlonglong progress, ushort info){
    m_currentProgress = progress;
    qDebug() << "onDeletingProcessAmount" << progress << info;
}

void DeletejobWorker::handleTimeout(){
    float speed = (m_currentProgress - m_lastProgress) / (1024 * 1024);
//    qDebug() << speed;
    m_lastProgress = m_currentProgress;
    int remainTime = (m_totalAmout - m_currentProgress) / speed;
//    qDebug() << remainTime;
    m_jobDataDetail.insert("speed", QString::number(speed));
    m_jobDataDetail.insert("remainTime", QString::number(remainTime));
    m_jobDataDetail.insert("progress", "10");
    emit signalManager->deleteJobDataUpdated(m_jobDetail, m_jobDataDetail);
}

void DeletejobWorker::handleFinished(){
    if (m_jobDetail.contains("jobPath")){
        emit signalManager->deleteJobRemoved(m_jobDetail);
    }
}

void DeletejobWorker::handleTaskAborted(const QMap<QString, QString> &jobDetail){
    if (jobDetail == m_jobDetail){
        deleteJobAbort();
    }
}

DeletejobWorker::~DeletejobWorker()
{

}
