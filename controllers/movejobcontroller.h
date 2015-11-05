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
    void createMoveJob(QStringList files, QString destination);
};

#endif // MOVEJOBCONTROLLER_H
