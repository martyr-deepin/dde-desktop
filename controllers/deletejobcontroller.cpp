#include "deletejobcontroller.h"
#include "dbusinterface/deletejob_interface.h"
#include "dbusinterface/fileoperations_interface.h"
#include "dialogs/confirmdeletedialog.h"
#include "views/global.h"

DeleteJobController::DeleteJobController(QObject *parent) : QObject(parent)
{
    initConnect();
}

void DeleteJobController::initConnect(){
    connect(signalManager, SIGNAL(deleteFilesExcuted(QStringList)),
            this, SLOT(confimDelete(QStringList)));
    connect(signalManager, SIGNAL(deleteJobAboutToAbort()), this, SLOT(deleteJobAbort()));
}


void DeleteJobController::confimDelete(const QStringList &files){
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
        message = tr("Are you sure to delete  ") + QFileInfo(m_deletefiles.at(0)).fileName();
    }else{
        message = tr("Are you sure to delete these ") +  QString::number(m_deletefiles.length()) + " items";
    }
    d.setMessage(message);
    connect(&d, SIGNAL(buttonClicked(int)), this, SLOT(handleDeleteAction(int)));
    int code = d.exec();
    qDebug() << code << "dialog close";
}

void DeleteJobController::handleDeleteAction(int index){
    switch (index) {
    case 0:
        return;
        break;
    case 1:
        deleteFiles(m_deletefiles);
        break;
    default:
        break;
    }
}


void DeleteJobController::deleteFiles(const QStringList &files){
    LOG_INFO() << "delete" <<files;
    QDBusPendingReply<QString, QDBusObjectPath, QString> reply = dbusController->getFileOperationsInterface()->NewDeleteJob(files, false, "", "", "");
    reply.waitForFinished();
    if (!reply.isError()){
        QString service = reply.argumentAt(0).toString();
        QString path = qdbus_cast<QDBusObjectPath>(reply.argumentAt(1)).path();
        LOG_INFO() << "delete files" << files << path;
        m_deleteJobInterface = new DeleteJobInterface(service, path, QDBusConnection::sessionBus(), this);
        connectDeleteJobSignal();
        m_deleteJobInterface->Execute();
    }else{
        LOG_ERROR() << reply.error().message();
    }
}

void DeleteJobController::connectDeleteJobSignal(){
    if (m_deleteJobInterface){
        connect(m_deleteJobInterface, SIGNAL(Done()), this, SLOT(deleteJobExcuteFinished()));
        connect(m_deleteJobInterface, SIGNAL(Aborted()), this, SLOT(deleteJobAbortFinished()));
        connect(m_deleteJobInterface, SIGNAL(Deleting(QString)), this, SLOT(onDeletingFile(QString)));
        connect(m_deleteJobInterface, SIGNAL(ProcessedAmount(qlonglong,ushort)),
                this, SLOT(onDeletingProcessAmount(qlonglong,ushort)));
    }
}


void DeleteJobController::disconnectDeleteJobSignal(){
    if (m_deleteJobInterface){
        disconnect(m_deleteJobInterface, SIGNAL(Done()), this, SLOT(deleteJobExcuteFinished()));
        disconnect(m_deleteJobInterface, SIGNAL(Aborted()), this, SLOT(deleteJobAbortFinished()));
        disconnect(m_deleteJobInterface, SIGNAL(Deleting(QString)), this, SLOT(onDeletingFile(QString)));
        disconnect(m_deleteJobInterface, SIGNAL(ProcessedAmount(qlonglong,ushort)),
                this, SLOT(onDeletingProcessAmount(qlonglong,ushort)));
    }
}


void DeleteJobController::deleteJobExcuteFinished(){
    disconnectDeleteJobSignal();
    m_deleteJobInterface->deleteLater();
    m_deleteJobInterface = NULL;
    m_deletefiles.clear();
    LOG_INFO() << "delete job finished";
}

void DeleteJobController::deleteJobAbort(){
    if (m_deleteJobInterface){
        m_deleteJobInterface->Abort();
    }
}

void DeleteJobController::deleteJobAbortFinished(){
    disconnectDeleteJobSignal();
    m_deleteJobInterface->deleteLater();
    m_deleteJobInterface = NULL;
    LOG_INFO() << "delete job aborted";
}


void DeleteJobController::onDeletingFile(QString file){
    emit signalManager->deletingFileChaned(file);
    LOG_INFO() << "onDeletingFile" << file;
}

void DeleteJobController::onDeletingProcessAmount(qlonglong progress, ushort info){
    emit signalManager->deletingProcessAmountChanged(progress, info);
    LOG_INFO() << "onDeletingProcessAmount" << progress << info;
}

DeleteJobController::~DeleteJobController()
{

}

