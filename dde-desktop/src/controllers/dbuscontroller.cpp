#include "dbuscontroller.h"
#include "views/global.h"


DBusController* DBusController::m_instance=NULL;

DBusController::DBusController(QObject *parent) : QObject(parent)
{
    QDBusConnection bus = QDBusConnection::sessionBus();
    m_monitorManagerInterface = new MonitorManagerInterface(FileMonitor_service, FileMonitor_path, bus);
    m_fileInfoInterface = new FileInfoInterface(FileInfo_service, FileInfo_path, bus);
    m_desktopDaemonInterface = new DesktopDaemonInterface(DesktopDaemon_service, DesktopDaemon_path, bus);
    getDesktopItems();
    monitorDesktop();
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
        qDebug() << "=========";
    }else{
        qDebug() << reply.error().message();
    }
}


void DBusController::asyncRenameDesktopItemByUrl(QString url){
    QDBusPendingReply<DesktopItemInfo> reply = m_desktopDaemonInterface->GetItemInfo(url);

    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(reply, this);
    connect(watcher, SIGNAL(finished(QDBusPendingCallWatcher*)),
                        this, SLOT(asyncRenameDesktopItemByUrlFinished(QDBusPendingCallWatcher*)));
}

void DBusController::asyncRenameDesktopItemByUrlFinished(QDBusPendingCallWatcher *call){
    QDBusPendingReply<DesktopItemInfo> reply = *call;
    if (!reply.isError()) {
        DesktopItemInfo desktopItemInfo = qdbus_cast<DesktopItemInfo>(reply.argumentAt(0));
        emit signalManager->itemMoved(desktopItemInfo);
    } else {
        qDebug() << reply.error().message();
    }
    call->deleteLater();
}


void DBusController::asyncCreateDesktopItemByUrl(QString url){
    QDBusPendingReply<DesktopItemInfo> reply = m_desktopDaemonInterface->GetItemInfo(url);

    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(reply, this);
    connect(watcher, SIGNAL(finished(QDBusPendingCallWatcher*)),
                        this, SLOT(asyncCreateDesktopItemByUrlFinished(QDBusPendingCallWatcher*)));
}


void DBusController::asyncCreateDesktopItemByUrlFinished(QDBusPendingCallWatcher *call){
    QDBusPendingReply<DesktopItemInfo> reply = *call;
    if (!reply.isError()) {
        DesktopItemInfo desktopItemInfo = qdbus_cast<DesktopItemInfo>(reply.argumentAt(0));
        emit signalManager->itemCreated(desktopItemInfo);
    } else {
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
        emit signalManager->itemDeleted(url);
        break;
    case G_FILE_MONITOR_EVENT_CREATED:
        asyncCreateDesktopItemByUrl(url);
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
        emit signalManager->itemShoudBeMoved(url);
        asyncRenameDesktopItemByUrl(in1);
        break;
    default:
        break;
    }
}

DBusController::~DBusController()
{

}

