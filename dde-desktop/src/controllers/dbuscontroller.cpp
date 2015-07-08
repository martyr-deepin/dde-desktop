#include "dbuscontroller.h"


DBusController::DBusController(QObject *parent) : QObject(parent)
{
    QDBusConnection bus = QDBusConnection::sessionBus();
    m_fileMonitorInterface = new FileMonitorInterface(FileMonitor_service, FileMonitor_path, bus);
    m_FileOperationsInterface = new FileOperationsInterface(FileOperations_service, FileOperations_path, bus);
    m_FileOperationsFlagsInterface = new FileOperationsFlagsInterface(FileOperations_service, FileOperations_path, bus);

//    QDBusObjectPath job;
//    QString out2;
//    QDBusReply<QString> reply = m_FileOperationsInterface->NewListJob("/home/djf/桌面/Terminator.desktop", 0, job, out2);
    QDBusPendingReply<QString, QDBusObjectPath, QString> reply = m_FileOperationsInterface->NewListJob("/home/djf/桌面/Terminator.desktop", 0);
    reply.waitForFinished();
    qDebug() << reply.isFinished();
    if (!reply.isError()){
        QString service = reply.argumentAt(0).toString();
        QString path = qdbus_cast<QDBusObjectPath>(reply.argumentAt(1)).path();
        FileListJobInterface* fileListJobInterface = new FileListJobInterface(service, path, bus);
        QDBusPendingReply<EntryInfoObjList> r =  fileListJobInterface->Execute();

    }else{
        qDebug() << reply.error().message() << "=======";
    }
}

DBusController::~DBusController()
{

}

