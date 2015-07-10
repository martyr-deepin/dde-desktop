#include "dbuscontroller.h"
#include "views/global.h"


DBusController* DBusController::m_instance=NULL;

DBusController::DBusController(QObject *parent) : QObject(parent)
{
    QDBusConnection bus = QDBusConnection::sessionBus();
    m_monitorManagerInterface = new MonitorManagerInterface(FileMonitor_service, FileMonitor_path, bus);
    m_FileOperationsInterface = new FileOperationsInterface(FileOperations_service, FileOperations_path, bus);
    m_FileOperationsFlagsInterface = new FileOperationsFlagsInterface(FileOperations_service, FileOperations_path, bus);
    m_desktopDaemonInterface = new DesktopDaemonInterface(DesktopDaemon_service, DesktopDaemon_path, bus);
    getOperationsFlags();
    monitorDesktop();

    call_FileOperations_NewListJob(desktopLocation, ListJobFlagIncludeHidden);
}


DBusController* DBusController::instance(){
    static QMutex mutex;
    if (!m_instance) {
        QMutexLocker locker(&mutex);
        if (!m_instance)
            m_instance = new DBusController;
    }
    return m_instance;
}

DesktopDaemonInterface* DBusController::getDesktopDaemonInterface(){
    return m_desktopDaemonInterface;
}

void DBusController::monitorDesktop(){
    QDBusPendingReply<QString, QDBusObjectPath, QString> reply = m_monitorManagerInterface->Monitor(desktopLocation, G_FILE_MONITOR_SEND_MOVED);
    reply.waitForFinished();
    if (!reply.isError()){
        QString service = reply.argumentAt(0).toString();
        QString path = qdbus_cast<QDBusObjectPath>(reply.argumentAt(1)).path();
        FileMonitorInstanceInterface* desktopMonitorInterface = new FileMonitorInstanceInterface(service, path, QDBusConnection::sessionBus());
        connect(desktopMonitorInterface, SIGNAL(Changed(QString,QString,uint)), this, SLOT(desktopFileChanged(QString,QString,uint)));
        qDebug() << service << path << "=========";
    }else{
        qDebug() << reply.error().message();
    }
}

void DBusController::getDesktopItems(){
    QDBusPendingReply<DesktopItemInfoMap> reply = m_desktopDaemonInterface->GetDesktopItems();
    reply.waitForFinished();
    if (!reply.isError()){
        DesktopItemInfoMap desktopItems = qdbus_cast<DesktopItemInfoMap>(reply.argumentAt(0));
        emit signalManager->desktopItemsChanged(desktopItems);
        qDebug() << desktopItems.count();
    }else{
        qDebug() << reply.error().message();
    }
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
    fileListJobInterface->deleteLater();
    // todo entryinfo
}


void DBusController::fileListJob_execute_finished(QDBusPendingCallWatcher *call){
    QDBusPendingReply<EntryInfoObjList> reply = *call;
    if (!reply.isError()) {
        EntryInfoObjList objList = qdbus_cast<EntryInfoObjList>(reply.argumentAt(0));
        emit signalManager->desktopItemsLoaded(objList);
        getDesktopItems();
    }else {
        qDebug() << reply.error().message();
    }
    call->deleteLater();
}


void DBusController::desktopFileChanged(const QString &url, const QString &in1, uint event){
    qDebug() << url << in1;
    switch (event) {
    case G_FILE_MONITOR_EVENT_CHANGED:
        qDebug() << "file content changed";
        break;
    case G_FILE_MONITOR_EVENT_CHANGES_DONE_HINT:
        qDebug() << "file event changed over";
        break;
    case G_FILE_MONITOR_EVENT_DELETED:
        qDebug() << "file deleted";
        break;
    case G_FILE_MONITOR_EVENT_CREATED:
        qDebug() << "file created";
        break;
    case G_FILE_MONITOR_EVENT_ATTRIBUTE_CHANGED:
        qDebug() << "file attribute changed";
        break;
    case G_FILE_MONITOR_EVENT_PRE_UNMOUNT:
        qDebug() << "file pre unmount";
        break;
    case G_FILE_MONITOR_EVENT_UNMOUNTED:
        qDebug() << "file event unmounted";
        break;
    case G_FILE_MONITOR_EVENT_MOVED:
        qDebug() << "file event moved";
        break;
    default:
        break;
    }
}

DBusController::~DBusController()
{

}

