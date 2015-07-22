#ifndef DBUSCONTROLLER_H
#define DBUSCONTROLLER_H

#include "dbusinterface/monitormanager_interface.h"
#include "dbusinterface/clipboard_interface.h"
#include "dbusinterface/watcherinstance_interface.h"
#include "dbusinterface/trashmonitor_interface.h"
#include "dbusinterface/filemonitorInstance_interface.h"
#include "dbusinterface/fileInfo_interface.h"
#include "dbusinterface/desktopdaemon_interface.h"
#include "dbusinterface/fileoperations_interface.h"
#include "dbusinterface/createdirjob_interface.h"
#include "dbusinterface/createfilejob_interface.h"
#include "dbusinterface/createfilefromtemplatejob_interface.h"
#include "dbusinterface/trashjob_interface.h"


#include "dbusinterface/dbustype.h"
#include "views/signalmanager.h"
#include <QtCore>
#include <QtDBus>

#define FileMonitor_service "com.deepin.filemanager.Backend.Monitor"
#define FileMonitor_path "/com/deepin/filemanager/Backend/MonitorManager"
#define FileOperations_path "/com/deepin/filemanager/Backend/Operations"
#define TrashMonitor_path "/com/deepin/filemanager/Backend/Monitor/TrashMonitor"
#define Clipboard_path "/com/deepin/filemanager/Backend/Clipboard"

#define FileInfo_service "com.deepin.filemanager.Backend.FileInfo"
#define FileInfo_path "/com/deepin/filemanager/Backend/FileInfo"

#define DesktopDaemon_service "com.deepin.dde.daemon.Desktop"
#define DesktopDaemon_path "/com/deepin/dde/daemon/Desktop"

#define Watcher_service "com.deepin.filemanager.Backend.Watcher"

typedef QSharedPointer<FileMonitorInstanceInterface> FileMonitorInstanceInterfacePointer;

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
    void watchDesktop();
    void requestDesktopItems();
    void requestIconByUrl(QString scheme, uint size);

    FileOperationsInterface* getFileOperationsInterface();
    FileInfoInterface* getFileInfoInterface();

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
    void watchFileChanged(QString url, uint event);

    void appGroupFileChanged(const QString &url, const QString &in1, uint event);
    void asyncRenameDesktopItemByUrlFinished(QDBusPendingCallWatcher* call);
    void asyncCreateDesktopItemByUrlFinished(QDBusPendingCallWatcher* call);

    void openFiles(QStringList files, IntList intFlags);
    void openFiles(DesktopItemInfo destinationDesktopItemInfo, QList<DesktopItemInfo> desktopItemInfos);
    void openFile(DesktopItemInfo desktopItemInfo);

    /*create file*/
    void createDirectory();
    void createDirectoryFinished(QString dirName);
    void createFile();
    void createFileFinished(QString filename);
    void createFileFromTemplate(QString templatefile);
    void createFileFromTemplateFinished(QString filename);
    void sortByKey(QString key);

    /*app group*/
    void requestCreatingAppGroup(QStringList urls);
    void monitorAppGroup(QString group_url);
    void getAppGroupItemsByUrl(QString group_url);
    void createAppGroup(QString group_url, QStringList urls);
    void mergeIntoAppGroup(QStringList urls, QString group_url);

    /*unmonitor file*/
    void unMonitorDirByID(uint id);
    void unMonitorDirByUrl(QString group_url);
    void unMonitor();

    /*paste files*/
    void pasteFiles(QString action, QStringList files, QString destination);

private:
    DBusController(QObject *parent = 0);
    ~DBusController();
    MonitorManagerInterface* m_monitorManagerInterface = NULL;
    FileMonitorInstanceInterface* m_desktopMonitorInterface = NULL;
    ClipboardInterface* m_clipboardInterface = NULL;
    WatcherInstanceInterface* m_watchInstanceInterface = NULL;
    QMap<QString, FileMonitorInstanceInterfacePointer> m_appGroupMonitorInterfacePointers;
    FileInfoInterface* m_fileInfoInterface = NULL;
    DesktopDaemonInterface* m_desktopDaemonInterface = NULL;
    FileOperationsInterface* m_fileOperationsInterface = NULL;

    CreateDirJobInterface* m_createDirJobInterface = NULL;
    CreateFileJobInterface* m_createFileJobInterface = NULL;
    CreateFileFromTemplateJobInterface* m_createFileFromTemplateJobInterface = NULL;

    DesktopItemInfoMap m_desktopItemInfoMap;
    QMap<QString, DesktopItemInfoMap> m_appGroups;

    QString m_itemShoudBeMoved;

    static DBusController* m_instance;
    Q_DISABLE_COPY(DBusController)

};

#endif // DBUSCONTROLLER_H
