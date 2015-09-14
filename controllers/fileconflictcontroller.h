#ifndef FILECONFLICTCONTROLLER_H
#define FILECONFLICTCONTROLLER_H

#include <QObject>
#include <dbusinterface/services/conflictdaptor.h>
#include <dbusinterface/dbustype.h>

class FileConflictController : public QObject
{
    Q_OBJECT
public:
    explicit FileConflictController(QObject *parent = 0);
    ~FileConflictController();
    void registerDBusService();

signals:

public slots:
    ConflictInfo AskSkip(const QString &primaryText, const QString &secondaryText, const QString &detailText, int flags);
    ConflictInfo ConflictDialog();

public slots:

};

#endif // FILECONFLICTCONTROLLER_H
