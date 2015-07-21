#ifndef COPYJOBCONTROLLER_H
#define COPYJOBCONTROLLER_H

#include "dbusinterface/copyjob_interface.h"
#include <QtCore>
#include <QtDBus>

class CopyJobController : public QObject
{
    Q_OBJECT
public:
    explicit CopyJobController(QObject *parent = 0);
    ~CopyJobController();

    void initConnect();

signals:

public slots:
    void copyFiles(QStringList files, QString destination);

    void connectCopyJobSignal();
    void disconnectCopyJobSignal();
    void copyJobExcuteFinished(QString file);
    void copyJobAbort();
    void copyJobAbortFinished();
    void onCopyingFile(QString file);
    void onCopyingProcessAmount(qulonglong progress, ushort info);

private:
    CopyJobInterface* m_copyJobInterface;
};

#endif // COPYJOBCONTROLLER_H
