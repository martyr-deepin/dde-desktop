#ifndef MOVEJOBCONTROLLER_H
#define MOVEJOBCONTROLLER_H

#include "dbusinterface/movejob_interface.h"
#include <QtCore>
#include <QtDBus>

class MoveJobController : public QObject
{
    Q_OBJECT
public:
    explicit MoveJobController(QObject *parent = 0);
    ~MoveJobController();

    void initConnect();

signals:

public slots:
    void moveFiles(QStringList files, QString destination);
    void connectMoveJobSignal();
    void disconnectMoveJobSignal();
    void moveJobExcuteFinished();
    void moveJobAbort();
    void moveJobAbortFinished();
    void onMovingFile(QString file);
    void onMovingProcessAmount(qulonglong progress, ushort info);

private:
    MoveJobInterface* m_moveJobInterface = NULL;
};

#endif // MOVEJOBCONTROLLER_H
