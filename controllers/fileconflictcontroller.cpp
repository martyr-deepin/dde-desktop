#include "fileconflictcontroller.h"
#include <QDBusConnection>
#include <QDebug>

FileConflictController::FileConflictController(QObject *parent) :
    QObject(parent)
{
    registerDBusService();
}

void FileConflictController::registerDBusService(){
    new ConflictAdaptor(this);
    QDBusConnection conn = QDBusConnection::sessionBus();
    conn.registerService(ConflictAdaptor::staticServerPath());
    bool flag = conn.registerObject(ConflictAdaptor::staticInterfacePath(), this);
    qDebug() << "registerDBusService"<<flag;
}


ConflictInfo FileConflictController::AskSkip(const QString &primaryText,
                                     const QString &secondaryText,
                                     const QString &detailText,
                                     int flags){
    Q_UNUSED(primaryText)
    Q_UNUSED(secondaryText)
    Q_UNUSED(detailText)
    Q_UNUSED(flags)
    ConflictInfo obj;
    obj.code = 1;
    obj.applyToAll = false;
    obj.userData = "";
    return obj;
}


ConflictInfo FileConflictController::ConflictDialog(){
    qDebug() << "=====ConflictDialog======";
    ConflictInfo obj;
    obj.code = 32;
    obj.applyToAll = false;
    obj.userData = "";
    return obj;
}

FileConflictController::~FileConflictController()
{
    QDBusConnection conn = QDBusConnection::sessionBus();
    conn.unregisterObject(ConflictAdaptor::staticInterfacePath());
    conn.registerService(ConflictAdaptor::staticServerPath());
}
