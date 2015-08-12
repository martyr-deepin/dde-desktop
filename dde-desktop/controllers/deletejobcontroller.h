#ifndef DELETEJOBCONTROLLER_H
#define DELETEJOBCONTROLLER_H

#include <QObject>

#include <QtCore>
#include <QtDBus>

class DeleteJobInterface;

class DeleteJobController : public QObject
{
    Q_OBJECT
public:
    explicit DeleteJobController(QObject *parent = 0);
    ~DeleteJobController();

    void initConnect();

signals:

public slots:
    void deleteFiles(const QStringList& files);
    void connectDeleteJobSignal();
    void disconnectDeleteJobSignal();
    void deleteJobExcuteFinished();
    void deleteJobAbort();
    void deleteJobAbortFinished();
    void onDeletingFile(QString file);
    void onDeletingProcessAmount(qulonglong progress, ushort info);

    void confimDelete(const QStringList& files);
    void handleDeleteAction(int index);

private:
    DeleteJobInterface* m_deleteJobInterface = NULL;
    QStringList m_deletefiles;
};

#endif // DELETEJOBCONTROLLER_H
