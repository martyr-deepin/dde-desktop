#include "dbuscontroller.h"
#include "views/global.h"


DBusController* DBusController::m_instance=NULL;

DBusController::DBusController(QObject *parent) : QObject(parent)
{
    QDBusConnection bus = QDBusConnection::sessionBus();
    m_monitorManagerInterface = new MonitorManagerInterface(FileMonitor_service, FileMonitor_path, bus, this);
    m_desktopMonitorInterface = NULL;
    m_fileInfoInterface = new FileInfoInterface(FileInfo_service, FileInfo_path, bus, this);
    m_desktopDaemonInterface = new DesktopDaemonInterface(DesktopDaemon_service, DesktopDaemon_path, bus, this);
    m_fileOperationsInterface = new FileOperationsInterface(FileMonitor_service, FileOperations_path, bus, this);
    m_createDirJobInterface = NULL;
    m_createFileJobInterface = NULL;
    m_createFileFromTemplateJobInterface = NULL;
    getDesktopItems();
    monitorDesktop();
    initConnect();
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


void DBusController::initConnect(){
    connect(m_desktopDaemonInterface, SIGNAL(RequestOpen(QStringList)), this, SLOT(openFiles(QStringList)));
    connect(m_desktopDaemonInterface, SIGNAL(RequestCreateDirectory()), this, SLOT(createDirectory()));
    connect(m_desktopDaemonInterface, SIGNAL(RequestCreateFile()), this, SLOT(createFile()));
    connect(m_desktopDaemonInterface, SIGNAL(RequestCreateFileFromTemplate(QString)), this, SLOT(createFileFromTemplate(QString)));
    connect(m_desktopDaemonInterface, SIGNAL(RequestSort(QString)),this, SLOT(sortByKey(QString)));
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
        m_desktopMonitorInterface = new FileMonitorInstanceInterface(service, path, QDBusConnection::sessionBus(), this);
        connect(m_desktopMonitorInterface, SIGNAL(Changed(QString,QString,uint)), this, SLOT(desktopFileChanged(QString,QString,uint)));
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
        m_desktopItemInfoMap = desktopItems;
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
        updateDesktopItemInfoMap(desktopItemInfo);
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
        updateDesktopItemInfoMap(desktopItemInfo);
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
        removeDesktopItemInfoByUrl(url);
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
        m_itemShoudBeMoved = url;
        emit signalManager->itemShoudBeMoved(url);
        asyncRenameDesktopItemByUrl(in1);
        break;
    default:
        break;
    }
}


void DBusController::updateDesktopItemInfoMap(DesktopItemInfo desktopItemInfo){
    m_desktopItemInfoMap.insert(desktopItemInfo.URI, desktopItemInfo);
}


void DBusController::updateDesktopItemInfoMap_moved(DesktopItemInfo desktopItemInfo){
    QString oldKey = m_itemShoudBeMoved;
    QString newKey = desktopItemInfo.URI;

    DesktopItemInfoMap::Iterator iterator = m_desktopItemInfoMap.find(oldKey);
    if (iterator!=m_desktopItemInfoMap.end()){
        m_desktopItemInfoMap.insert(iterator, newKey, desktopItemInfo);
    }
}

void DBusController::removeDesktopItemInfoByUrl(QString url){
    if (m_desktopItemInfoMap.contains(url)){
        m_desktopItemInfoMap.remove(url);
    }
}

void DBusController::openFiles(QStringList files){
    foreach (QString file, files) {
        QString key = QString(QUrl(file.toLocal8Bit()).toEncoded());
        if (m_desktopItemInfoMap.contains(key)){
            DesktopItemInfo desktopItemInfo = m_desktopItemInfoMap.value(key);
//            m_desktopDaemonInterface->ActivateFile(desktopItemInfo.URI, QStringList(), desktopItemInfo.CanExecute, )
        }
    }
}

void DBusController::createDirectory(){
    QDBusPendingReply<QString, QDBusObjectPath, QString> reply = m_fileOperationsInterface->NewCreateDirectoryJob(desktopLocation, "", "", "", "");
    reply.waitForFinished();
    if (!reply.isError()){
        QString service = reply.argumentAt(0).toString();
        QString path = qdbus_cast<QDBusObjectPath>(reply.argumentAt(1)).path();
        m_createDirJobInterface = new CreateDirJobInterface(service, path, QDBusConnection::sessionBus(), this);
        connect(m_createDirJobInterface, SIGNAL(Done(QString)), this, SLOT(createDirectoryFinished(QString)));
        m_createDirJobInterface->Execute();
    }else{
        qDebug() << reply.error().message();
    }
}


void DBusController::createDirectoryFinished(QString dirName){
    Q_UNUSED(dirName)
    disconnect(m_createDirJobInterface, SIGNAL(Done(QString)), this, SLOT(createDirectoryFinished(QString)));
    m_createDirJobInterface = NULL;
}

void DBusController::createFile(){
    QDBusPendingReply<QString, QDBusObjectPath, QString> reply = m_fileOperationsInterface->NewCreateFileJob(desktopLocation, "", "", "", "", "");
    reply.waitForFinished();
    if (!reply.isError()){
        QString service = reply.argumentAt(0).toString();
        QString path = qdbus_cast<QDBusObjectPath>(reply.argumentAt(1)).path();
        m_createFileJobInterface = new CreateFileJobInterface(service, path, QDBusConnection::sessionBus(), this);
        connect(m_createFileJobInterface, SIGNAL(Done(QString)), this, SLOT(createFileFinished(QString)));
        m_createFileJobInterface->Execute();
    }else{
        qDebug() << reply.error().message();
    }
}

void DBusController::createFileFinished(QString filename){
    Q_UNUSED(filename)
    disconnect(m_createFileJobInterface, SIGNAL(Done(QString)), this, SLOT(createFileFinished(QString)));
    m_createFileJobInterface = NULL;
}


void DBusController::createFileFromTemplate(QString templatefile){
    QDBusPendingReply<QString, QDBusObjectPath, QString> reply = m_fileOperationsInterface->NewCreateFileFromTemplateJob(desktopLocation, templatefile, "", "", "");
    reply.waitForFinished();
    if (!reply.isError()){
        QString service = reply.argumentAt(0).toString();
        QString path = qdbus_cast<QDBusObjectPath>(reply.argumentAt(1)).path();
        m_createFileFromTemplateJobInterface = new CreateFileFromTemplateJobInterface(service, path, QDBusConnection::sessionBus(), this);
        connect(m_createFileFromTemplateJobInterface, SIGNAL(Done(QString)), this, SLOT(createFileFromTemplateFinished(QString)));
        m_createFileFromTemplateJobInterface->Execute();
    }else{
        qDebug() << reply.error().message();
    }
}


void DBusController::createFileFromTemplateFinished(QString filename){
    Q_UNUSED(filename)
    disconnect(m_createFileFromTemplateJobInterface, SIGNAL(Done(QString)), this, SLOT(createFileFromTemplateFinished(QString)));
    m_createFileFromTemplateJobInterface = NULL;
}


void DBusController::sortByKey(QString key){
    qDebug() << key;
    emit signalManager->sortByKey(key);
}

DBusController::~DBusController()
{

}

