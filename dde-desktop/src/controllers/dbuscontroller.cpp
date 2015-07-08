#include "dbuscontroller.h"


DBusController::DBusController(QObject *parent) : QObject(parent)
{
    QDBusConnection bus = QDBusConnection::sessionBus();
    m_fileMonitorInterface = new FileMonitorInterface(FileMonitor_service, FileMonitor_path, bus);
    m_FileOperationsInterface = new FileOperationsInterface(FileOperations_service, FileOperations_path, bus);
    m_FileOperationsFlagsInterface = new FileOperationsFlagsInterface(FileOperations_service, FileOperations_path, bus);

    getOperationsFlags();

    call_FileOperations_NewListJob("/home/djf/桌面", ListJobFlagIncludeHidden);
}

void DBusController::getOperationsFlags(){
    ListJobFlagIncludeHidden = m_FileOperationsFlagsInterface->listJobFlagIncludeHidden();
    ListJobFlagNone = m_FileOperationsFlagsInterface->listJobFlagRecusive();
    ListJobFlagRecusive = m_FileOperationsFlagsInterface->listJobFlagRecusive();
    CopyFlagNofollowSymlinks = m_FileOperationsFlagsInterface->copyFlagNofollowSymlinks();
    CopyFlagNone = m_FileOperationsFlagsInterface->copyFlagNone();
}

void DBusController::call_FileOperations_NewListJob(QString path, int flag){
    if (QDir(path).exists()){
        QDBusPendingReply<QString, QDBusObjectPath, QString> reply = m_FileOperationsInterface->NewListJob(path, flag);
        QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(reply, this);
        connect(watcher, SIGNAL(finished(QDBusPendingCallWatcher*)),this, SLOT(newListJob_finished(QDBusPendingCallWatcher*)));
    }
}

void DBusController::newListJob_finished(QDBusPendingCallWatcher *call)
{
    QDBusPendingReply<QString, QDBusObjectPath, QString> reply = *call;
    if (!reply.isError()) {
        QString service = reply.argumentAt(0).toString();
        QString path = qdbus_cast<QDBusObjectPath>(reply.argumentAt(1)).path();
        call_FileListJob_execute(service, path);
    }else {
        qDebug() << reply.error().message();
    }
    call->deleteLater();
}

void DBusController::call_FileListJob_execute(const QString &service, const QString &path){
    QDBusConnection bus = QDBusConnection::sessionBus();
    FileListJobInterface* fileListJobInterface = new FileListJobInterface(service, path, bus);
    QDBusPendingReply<EntryInfoObjList> reply =  fileListJobInterface->Execute();
    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(reply, this);
    connect(watcher, SIGNAL(finished(QDBusPendingCallWatcher*)),this, SLOT(fileListJob_execute_finished(QDBusPendingCallWatcher*)));
}


void DBusController::fileListJob_execute_finished(QDBusPendingCallWatcher *call){
    QDBusPendingReply<EntryInfoObjList> reply = *call;
    if (!reply.isError()) {
        EntryInfoObjList objList = qdbus_cast<EntryInfoObjList>(reply.argumentAt(0));
        foreach (EntryInfoObj obj, objList) {
            qDebug() << obj.DisplayName << obj.Icon;
        }
    }else {
        qDebug() << reply.error().message();
    }
    call->deleteLater();
}


DBusController::~DBusController()
{

}

