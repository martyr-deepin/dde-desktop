#ifndef DBUSCONTROLLER_H
#define DBUSCONTROLLER_H

#include "dbusinterface/filemonitor_interface.h"
#include "dbusinterface/fileoperations_interface.h"
#include "dbusinterface/fileoperationsflags_interface.h"
#include "dbusinterface/filelistjob_interface.h"
#include "views/signalmanager.h"
#include <QObject>
#include <QtDBus/QtDBus>

#define FileMonitor_service "com.deepin.filemanager.Backend.Monitor"
#define FileMonitor_path "/com/deepin/filemanager/Backend/Monitor"

#define FileOperations_service "com.deepin.filemanager.Backend.Operations"
#define FileOperations_path "/com/deepin/filemanager/Backend/Operations"

class DBusController : public QObject
{
    Q_OBJECT
public:
    explicit DBusController(QObject *parent = 0);
    ~DBusController();

    void getOperationsFlags();

    void call_FileOperations_NewListJob(QString path, int flag);
    void call_FileListJob_execute(const QString& service, const QString& path);

signals:

public slots:
    void newListJob_finished(QDBusPendingCallWatcher *call);
    void fileListJob_execute_finished(QDBusPendingCallWatcher *call);

private:
    FileMonitorInterface* m_fileMonitorInterface;
    FileOperationsInterface* m_FileOperationsInterface;
    FileOperationsFlagsInterface* m_FileOperationsFlagsInterface;

    int ListJobFlagIncludeHidden;
    int ListJobFlagNone;
    int ListJobFlagRecusive;
    uint CopyFlagNofollowSymlinks;
    uint CopyFlagNone;
};

#endif // DBUSCONTROLLER_H
