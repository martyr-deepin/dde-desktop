#ifndef DBUSCONTROLLER_H
#define DBUSCONTROLLER_H

#include "dbusinterface/monitormanager_interface.h"
#include "dbusinterface/filemonitorInstance_interface.h"
#include "dbusinterface/fileoperations_interface.h"
#include "dbusinterface/fileoperationsflags_interface.h"
#include "dbusinterface/filelistjob_interface.h"
#include "views/signalmanager.h"
#include <QObject>
#include <QtDBus/QtDBus>

#define FileMonitor_service "com.deepin.filemanager.Backend.Monitor"
#define FileMonitor_path "/com/deepin/filemanager/Backend/MonitorManager"

#define FileOperations_service "com.deepin.filemanager.Backend.Operations"
#define FileOperations_path "/com/deepin/filemanager/Backend/Operations"

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
    explicit DBusController(QObject *parent = 0);
    ~DBusController();

    void monitorDesktop();

    void call_FileOperations_NewListJob(QString path, int flag); //
    void call_FileListJob_execute(const QString& service, const QString& path);

signals:

public slots:
    void newListJob_finished(QDBusPendingCallWatcher *call);
    void fileListJob_execute_finished(QDBusPendingCallWatcher *call);
    void desktopFileChanged(const QString &url, const QString &in1, uint event);

private:
    MonitorManagerInterface* m_monitorManagerInterface;
    FileOperationsInterface* m_FileOperationsInterface;
    FileOperationsFlagsInterface* m_FileOperationsFlagsInterface;

    int ListJobFlagIncludeHidden;
    int ListJobFlagNone;
    int ListJobFlagRecusive;
    uint CopyFlagNofollowSymlinks;
    uint CopyFlagNone;

    void getOperationsFlags();
};

#endif // DBUSCONTROLLER_H
