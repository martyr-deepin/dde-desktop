#ifndef APPCONTROLLER_H
#define APPCONTROLLER_H


#include <QObject>

class MenuController;
class TrashJobController;
class MoveJobController;
class CopyJobController;
class RenameJobController;
class DeleteJobController;
class FileConflictController;

class AppController : public QObject
{
    Q_OBJECT
public:
    explicit AppController(QObject *parent = 0);
    ~AppController();

    void initConnect();

signals:

public slots:



private:
    MenuController* m_menuController;
    TrashJobController* m_trashJobController;
    MoveJobController* m_moveJobController;
    CopyJobController* m_copyJobController;
    RenameJobController* m_renameJobController;
    DeleteJobController* m_deleteJobController;
    FileConflictController* m_fileConflictController;
};

#endif // APPCONTROLLER_H
