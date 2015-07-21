#ifndef TRASHJOBCONTROLLER_H
#define TRASHJOBCONTROLLER_H

#include "dbusinterface/trashjob_interface.h"
#include "dbusinterface/trashmonitor_interface.h"
#include <QtCore>
#include <QtDBus>

class TrashJobController : public QObject
{
    Q_OBJECT
public:
    explicit TrashJobController(QObject *parent = 0);
    ~TrashJobController();

    void initConnect();
    void monitorTrash();

signals:

public slots:
    void setTrashEmptyFlag(bool flag);
    void connectTrashSignal();
    void disconnectTrashSignal();
    void trashJobExcute(QStringList files);
    void trashJobExcuteFinished();
    void trashJobAbort();
    void trashJobAbortFinished();
    void onTrashingFile(QString file);
    void onDeletingFile(QString file);
    void onProcessAmount(qlonglong progress, ushort info);
    void updateTrashIconByCount(uint count);
private:
    TrashMonitorInterface* m_trashMonitorInterface=NULL;
    TrashJobInterface* m_trashJobInterface = NULL;
    bool m_isTrashEmpty = true;
};

#endif // TRASHJOBCONTROLLER_H
