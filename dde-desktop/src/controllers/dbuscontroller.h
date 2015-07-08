#ifndef DBUSCONTROLLER_H
#define DBUSCONTROLLER_H

#include "dbusinterface/filemonitor_interface.h"
#include "dbusinterface/fileoperations_interface.h"
#include "dbusinterface/fileoperationsflags_interface.h"
#include "dbusinterface/filelistjob_interface.h"
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

signals:

public slots:
private:
    FileMonitorInterface* m_fileMonitorInterface;
    FileOperationsInterface* m_FileOperationsInterface;
    FileOperationsFlagsInterface* m_FileOperationsFlagsInterface;
};

#endif // DBUSCONTROLLER_H
