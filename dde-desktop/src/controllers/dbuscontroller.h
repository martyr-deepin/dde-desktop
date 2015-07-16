#ifndef DBUSCONTROLLER_H
#define DBUSCONTROLLER_H

#include "dbusinterface/monitormanager_interface.h"
#include "dbusinterface/filemonitorInstance_interface.h"
#include "dbusinterface/fileInfo_interface.h"
#include "dbusinterface/desktopdaemon_interface.h"
#include "dbusinterface/fileoperations_interface.h"
#include "dbusinterface/createdirjob_interface.h"
#include "dbusinterface/createfilejob_interface.h"
#include "dbusinterface/createfilefromtemplatejob_interface.h"

#include "dbusinterface/dbustype.h"
#include "views/signalmanager.h"
#include <QObject>
#include <QtDBus/QtDBus>

#define FileMonitor_service "com.deepin.filemanager.Backend.Monitor"
#define FileMonitor_path "/com/deepin/filemanager/Backend/MonitorManager"
#define FileOperations_path "/com/deepin/filemanager/Backend/Operations"

#define FileInfo_service "com.deepin.filemanager.Backend.FileInfo"
#define FileInfo_path "com/deepin/filemanager/Backend/FileInfo"

#define DesktopDaemon_service "com.deepin.dde.daemon.Desktop"
#define DesktopDaemon_path "/com/deepin/dde/daemon/Desktop"

class DBusController : public QObject
{
    Q_OBJECT

    enum G_FILE_MONITOR_EVENT{
        G_FILE_MONITOR_EVENT_CHANGED,
        G_FILE_MONITOR_EVENT_CHANGES_DONE_HINT,
        G_FILE_MONITOR_EVENT_DELETED,
        G_FILE_MONITOR_EVENT_CREATED,
        G_FILE_MONITOR_EVENT_ATTRIBUTE_CHANGED,
        G_FILE_MONITOR_EVENT_PRE_UNMOUNT,
        G_FILE_MONITOR_EVENT_UNMOUNTED,
        G_FILE_MONITOR_EVENT_MOVED
    };

    enum G_FILE_MONITOR_FLAG{
        G_FILE_MONITOR_NONE = 0x0,
        G_FILE_MONITOR_WATCH_MOUNTS = 0x01,
        G_FILE_MONITOR_SEND_MOVED = 0x02,
        G_FILE_MONITOR_WATCH_HARD_LINKS = 0x04
    };

public:
    static DBusController* instance();
    void initConnect();
    void monitorDesktop();
    void getDesktopItems();
    QMap<QString, DesktopItemInfoMap> getAppGroups();

    DesktopItemInfoMap getDesktopItemInfoMap();
    void asyncRenameDesktopItemByUrl(QString url);
    void asyncCreateDesktopItemByUrl(QString url);

    DesktopDaemonInterface* getDesktopDaemonInterface();

    void updateDesktopItemInfoMap(DesktopItemInfo desktopItemInfo);
    void updateDesktopItemInfoMap_moved(DesktopItemInfo desktopItemInfo);
    void removeDesktopItemInfoByUrl(QString url);

signals:

public slots:
    void desktopFileChanged(const QString &url, const QString &in1, uint event);
    void asyncRenameDesktopItemByUrlFinished(QDBusPendingCallWatcher* call);
    void asyncCreateDesktopItemByUrlFinished(QDBusPendingCallWatcher* call);

    void openFiles(QStringList files, IntList intFlags);

    void createDirectory();
    void createDirectoryFinished(QString dirName);
    void createFile();
    void createFileFinished(QString filename);
    void createFileFromTemplate(QString templatefile);
    void createFileFromTemplateFinished(QString filename);
    void sortByKey(QString key);

    void requestCreatingAppGroup(QStringList urls);

    void getAppGroupItems(QString group_url);

private:
    DBusController(QObject *parent = 0);
    ~DBusController();
    MonitorManagerInterface* m_monitorManagerInterface;
    FileMonitorInstanceInterface* m_desktopMonitorInterface;
    FileInfoInterface* m_fileInfoInterface;
    DesktopDaemonInterface* m_desktopDaemonInterface;
    FileOperationsInterface* m_fileOperationsInterface;

    CreateDirJobInterface* m_createDirJobInterface;
    CreateFileJobInterface* m_createFileJobInterface;
    CreateFileFromTemplateJobInterface* m_createFileFromTemplateJobInterface;

    DesktopItemInfoMap m_desktopItemInfoMap;
    QMap<QString, DesktopItemInfoMap> m_appGroups;

    QString m_itemShoudBeMoved;
    static DBusController* m_instance;
    Q_DISABLE_COPY(DBusController)

};

#endif // DBUSCONTROLLER_H
