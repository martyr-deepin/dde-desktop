#ifndef APPCONTROLLER_H
#define APPCONTROLLER_H


#include <QObject>

class MenuController;
class TrashJobController;
class MoveJobController;
class CopyJobController;
class RenameJobController;

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
};

#endif // APPCONTROLLER_H
