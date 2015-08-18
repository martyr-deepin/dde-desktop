#ifndef MOVEJOBCONTROLLER_H
#define MOVEJOBCONTROLLER_H


#include <QtCore>
#include <QtDBus>

class MoveJobInterface;

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
    void onMovingProcessAmount(qlonglong progress, ushort info);

private:
    MoveJobInterface* m_moveJobInterface = NULL;
};

#endif // MOVEJOBCONTROLLER_H
