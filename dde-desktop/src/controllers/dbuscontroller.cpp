#include "dbuscontroller.h"
#include "views/global.h"


DBusController* DBusController::m_instance=NULL;

DBusController::DBusController(QObject *parent) : QObject(parent)
{
    QDBusConnection bus = QDBusConnection::sessionBus();
    m_monitorManagerInterface = new MonitorManagerInterface(FileMonitor_service, FileMonitor_path, bus, this);
    m_fileInfoInterface = new FileInfoInterface(FileInfo_service, FileInfo_path, bus, this);
    m_desktopDaemonInterface = new DesktopDaemonInterface(DesktopDaemon_service, DesktopDaemon_path, bus, this);
    m_fileOperationsInterface = new FileOperationsInterface(FileMonitor_service, FileOperations_path, bus, this);
    m_clipboardInterface = new ClipboardInterface(FileMonitor_service, Clipboard_path, bus, this);

    requestDesktopItems();
    requestIconByUrl(ComputerUrl, 48);
    requestIconByUrl(TrashUrl, 48);
    monitorDesktop();

//    watchDesktop();
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
    connect(m_desktopDaemonInterface, SIGNAL(RequestOpen(QStringList,IntList)),
            this, SLOT(openFiles(QStringList, IntList)));
    connect(m_desktopDaemonInterface, SIGNAL(RequestCreateDirectory()), this, SLOT(createDirectory()));
    connect(m_desktopDaemonInterface, SIGNAL(RequestCreateFile()), this, SLOT(createFile()));
    connect(m_desktopDaemonInterface, SIGNAL(RequestCreateFileFromTemplate(QString)),
            this, SLOT(createFileFromTemplate(QString)));
    connect(m_desktopDaemonInterface, SIGNAL(RequestSort(QString)),this, SLOT(sortByKey(QString)));

    connect(m_desktopDaemonInterface, SIGNAL(AppGroupCreated(QString,QStringList)),
            this, SLOT(createAppGroup(QString,QStringList)));

    connect(m_desktopDaemonInterface, SIGNAL(ItemCut(QStringList)),
            signalManager, SIGNAL(filesCuted(QStringList)));
    connect(m_desktopDaemonInterface, SIGNAL(RequestDelete(QStringList)),
            signalManager, SIGNAL(trashingAboutToExcute(QStringList)));

    connect(m_clipboardInterface, SIGNAL(RequestPaste(QString,QStringList,QString)),
            this, SLOT(pasteFiles(QString,QStringList,QString)));

    connect(signalManager, SIGNAL(requestCreatingAppGroup(QStringList)),
            this, SLOT(requestCreatingAppGroup(QStringList)));
    connect(signalManager, SIGNAL(requestMergeIntoAppGroup(QStringList,QString)),
            this, SLOT(mergeIntoAppGroup(QStringList,QString)));
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
        m_desktopMonitorInterface->SetRateLimit(100);
        connect(m_desktopMonitorInterface, SIGNAL(Changed(QString,QString,uint)), this, SLOT(desktopFileChanged(QString,QString,uint)));

    }else{
        qDebug() << reply.error().message();
    }
}


void DBusController::watchDesktop(){
    QDBusPendingReply<QString, QDBusObjectPath, QString> reply = m_monitorManagerInterface->Watch(desktopLocation);
    reply.waitForFinished();
    if (!reply.isError()){
        QString service = reply.argumentAt(0).toString();
        QString path = qdbus_cast<QDBusObjectPath>(reply.argumentAt(1)).path();
        m_watchInstanceInterface = new WatcherInstanceInterface(service, path, QDBusConnection::sessionBus(), this);
        connect(m_watchInstanceInterface, SIGNAL(Changed(QString,uint)), this, SLOT(watchFileChanged(QString,uint)));
    }else{
        qDebug() << reply.error().message();
    }
}

void DBusController::monitorAppGroup(QString group_url){
    if (!m_appGroupMonitorInterfacePointers.contains(group_url)){
        QDBusPendingReply<QString, QDBusObjectPath, QString> reply = m_monitorManagerInterface->Monitor(group_url, G_FILE_MONITOR_SEND_MOVED);
        reply.waitForFinished();
        if (!reply.isError()){
            QString service = reply.argumentAt(0).toString();
            QString path = qdbus_cast<QDBusObjectPath>(reply.argumentAt(1)).path();
            FileMonitorInstanceInterfacePointer interface = FileMonitorInstanceInterfacePointer::create(service, path, QDBusConnection::sessionBus(), this);
            interface->setProperty("group_url", group_url);
            m_appGroupMonitorInterfacePointers.insert(group_url, interface);
            connect(interface.data(), SIGNAL(Changed(QString,QString,uint)), this, SLOT(appGroupFileChanged(QString,QString,uint)));
        }else{
            qDebug() << reply.error().message();
        }
    }
}

void DBusController::requestDesktopItems(){
    QDBusPendingReply<DesktopItemInfoMap> reply = m_desktopDaemonInterface->GetDesktopItems();
    reply.waitForFinished();
    if (!reply.isError()){
        DesktopItemInfoMap desktopItems = qdbus_cast<DesktopItemInfoMap>(reply.argumentAt(0));
        emit signalManager->desktopItemsChanged(desktopItems);
        m_desktopItemInfoMap = desktopItems;

        foreach (QString url, desktopItems.keys()) {
            if (isAppGroup(decodeUrl(url))){
                getAppGroupItemsByUrl(url);
                monitorAppGroup(url);
            }
        }

    }else{
        qDebug() << reply.error().message();
    }
}

void DBusController::requestIconByUrl(QString scheme, uint size){
    QDBusPendingReply<QString> reply = m_fileInfoInterface->GetThemeIcon(scheme, size);
    reply.waitForFinished();
    if (!reply.isError()){
        QString iconUrl = reply.argumentAt(0).toString();
        emit signalManager->desktoItemIconUpdated(scheme, iconUrl, size);
    }else{
        qDebug() << reply.error().message();
    }
}

QMap<QString, DesktopItemInfoMap> DBusController::getAppGroups(){
    return m_appGroups;
}

FileOperationsInterface* DBusController::getFileOperationsInterface(){
    return m_fileOperationsInterface;
}

FileInfoInterface* DBusController::getFileInfoInterface(){
    return m_fileInfoInterface;
}

void DBusController::getAppGroupItemsByUrl(QString group_url){
    QString _group_url = group_url;
    QString group_dir = decodeUrl(_group_url.replace("file://", ""));
    QDir groupDir(group_dir);
    QFileInfoList fileInfoList  = groupDir.entryInfoList(QDir::NoDotAndDotDot | QDir::Files);
    if (groupDir.exists()){
        if (fileInfoList.count() == 0){
            bool flag = groupDir.removeRecursively();
            qDebug() << decodeUrl(group_url) << "delete" << flag;
            unMonitorDirByUrl(group_url);
        }else if (fileInfoList.count() == 1){
            qDebug() << fileInfoList.at(0).filePath() << "only one .desktop file in app group";
            QFile f(fileInfoList.at(0).filePath());
            QString newFileName = desktopLocation + QString(QDir::separator()) + fileInfoList.at(0).fileName();
            f.rename(newFileName);
            QDir(fileInfoList.at(0).absoluteDir()).removeRecursively();
            emit signalManager->appGounpDetailClosed();
            unMonitorDirByUrl(group_url);
        }else{

            QDBusPendingReply<DesktopItemInfoMap> reply = m_desktopDaemonInterface->GetAppGroupItems(group_url);
            reply.waitForFinished();
            if (!reply.isError()){
                DesktopItemInfoMap desktopItemInfos = qdbus_cast<DesktopItemInfoMap>(reply.argumentAt(0));

                if (desktopItemInfos.count() > 1){
                    emit signalManager->appGounpItemsChanged(group_url, desktopItemInfos);
                    m_appGroups.insert(group_url, desktopItemInfos);
                }else{
                    m_appGroups.remove(group_url);
                }
            }else{
                qDebug() << reply.error().message();
            }
        }
    }
}

DesktopItemInfoMap DBusController::getDesktopItemInfoMap(){
    return m_desktopItemInfoMap;
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

        if (isAppGroup(desktopItemInfo.URI)){
            getAppGroupItemsByUrl(desktopItemInfo.URI);
            monitorAppGroup(desktopItemInfo.URI);
        }

    } else {
        qDebug() << reply.error().message();
    }
    call->deleteLater();
}


void DBusController::watchFileChanged(QString url, uint event){
    qDebug() << url << event << "watchFile";
}


void DBusController::desktopFileChanged(const QString &url, const QString &in1, uint event){
    qDebug() << url << in1 << "desktop file changed!!!!!!!!!!";
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

        if (isAppGroup(url)){
            getAppGroupItemsByUrl(url);
            emit signalManager->appGounpDetailClosed();
        }
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
        qDebug() << "file event moved" << in1 << "========";
        if (in1.length() == 0){
            m_itemShoudBeMoved = url;
            emit signalManager->itemShoudBeMoved(url);
            asyncRenameDesktopItemByUrl(in1);
        }else{
            removeDesktopItemInfoByUrl(url);
            emit signalManager->itemDeleted(url);
        }
        break;
    default:
        break;
    }
}

void DBusController::appGroupFileChanged(const QString &url, const QString &in1, uint event){
    QString group_url = sender()->property("group_url").toString();
    switch (event) {
    case G_FILE_MONITOR_EVENT_CHANGED:
        qDebug() << "app group file content changed";
        break;
    case G_FILE_MONITOR_EVENT_CHANGES_DONE_HINT:
        qDebug() << "app group file event changed over";
        break;
    case G_FILE_MONITOR_EVENT_DELETED:
        qDebug() << "app group file deleted";
        getAppGroupItemsByUrl(group_url);
        break;
    case G_FILE_MONITOR_EVENT_CREATED:
        qDebug() << "app group file created";
        if(isApp(url)){
            getAppGroupItemsByUrl(group_url);
        }else{
            qDebug() << "*******" << "created invalid file(not .desktop file)"<< "*********";
        }
        break;
    case G_FILE_MONITOR_EVENT_ATTRIBUTE_CHANGED:
        qDebug() << "app group file attribute changed";
        break;
    case G_FILE_MONITOR_EVENT_PRE_UNMOUNT:
        qDebug() << "app group file pre unmount";
        break;
    case G_FILE_MONITOR_EVENT_UNMOUNTED:
        qDebug() << "app group file event unmounted";
        break;
    case G_FILE_MONITOR_EVENT_MOVED:
        if (QUrl(decodeUrl(QFileInfo(in1).path())).path() == desktopLocation){
            asyncCreateDesktopItemByUrl(in1); /*if app group has only one desktop file, move item to desktop and delete app group*/
        }
        getAppGroupItemsByUrl(group_url);
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

void DBusController::openFiles(QStringList files, IntList intFlags){
    qDebug() << files << intFlags;
    foreach (QString file, files) {
        QString key = QString(QUrl(file.toLocal8Bit()).toEncoded());
        if (m_desktopItemInfoMap.contains(key)){
            int index = files.indexOf(file);
            DesktopItemInfo desktopItemInfo = m_desktopItemInfoMap.value(key);
            qDebug() << desktopItemInfo.URI << "open";
            QDBusPendingReply<> reply = m_desktopDaemonInterface->ActivateFile(desktopItemInfo.URI, QStringList(), desktopItemInfo.CanExecute, 0);
            reply.waitForFinished();
            if (!reply.isError()){

            }else{
                qDebug() << reply.error().message();
            }
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


void DBusController::requestCreatingAppGroup(QStringList urls){
    QDBusPendingReply<> reply = m_desktopDaemonInterface->RequestCreatingAppGroup(urls);
    reply.waitForFinished();
    if (!reply.isError()){

    }else{
        qDebug() << reply.error().message();
    }
}

void DBusController::createAppGroup(QString group_url, QStringList urls){
//    qDebug() << group_url << urls;
//    if (urls.count() >= 2){
//        emit signalManager->appGounpCreated(group_url);
//    }
}

void DBusController::mergeIntoAppGroup(QStringList urls, QString group_url){
    qDebug() << urls << "merge into" << group_url;
    QDBusPendingReply<> reply = m_desktopDaemonInterface->RequestMergeIntoAppGroup(urls, group_url);
    reply.waitForFinished();
    if (!reply.isError()){
        getAppGroupItemsByUrl(group_url);
    }else{
        qDebug() << reply.error().message();
    }
}

void DBusController::unMonitorDirByID(uint id){
    qDebug() << "unMonitorDirByID:" << id;
    QDBusPendingReply<> reply = m_monitorManagerInterface->Unmonitor(id);
    reply.waitForFinished();
    if (!reply.isError()){

    }else{
        qDebug() << reply.error().message();
    }
}

void DBusController::unMonitorDirByUrl(QString group_url){
    if (m_appGroupMonitorInterfacePointers.contains(group_url)){
       unMonitorDirByID(m_appGroupMonitorInterfacePointers.value(group_url)->iD());
    }
}

void DBusController::unMonitor(){
    unMonitorDirByID(m_desktopMonitorInterface->iD());
    foreach (FileMonitorInstanceInterfacePointer p, m_appGroupMonitorInterfacePointers.values()) {
        unMonitorDirByID(p->iD());
    }
}


void DBusController::pasteFiles(QString action, QStringList files, QString destination){
    if (action == "cut"){
        bool isFilesFromDesktop = true;
        foreach (QString fpath, files) {
            QString url = fpath.replace("file://", "");
            QFileInfo f(url);
            bool flag = (f.absolutePath() == destination);
            isFilesFromDesktop = isFilesFromDesktop && flag;
        }
        if (isFilesFromDesktop){
            emit signalManager->cancelFilesCuted(files);
        }else{
            emit signalManager->moveFilesExcuted(files, destination);
        }
    }else if (action == "copy"){
        emit signalManager->copyFilesExcuted(files, destination);
    }
}


DBusController::~DBusController()
{

}

