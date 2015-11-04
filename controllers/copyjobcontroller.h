#ifndef COPYJOBCONTROLLER_H
#define COPYJOBCONTROLLER_H

#include <QtCore>
#include <QtDBus>

class CopyJobInterface;

class CopyJobController : public QObject
{
    Q_OBJECT
public:
    explicit CopyJobController(QObject *parent = 0);
    ~CopyJobController();

    void initConnect();

signals:

public slots:
    void createCopyJob(QStringList files, QString destination);
private:

};

#endif // COPYJOBCONTROLLER_H
