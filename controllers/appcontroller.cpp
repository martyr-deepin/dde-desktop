#include "appcontroller.h"
#include "menucontroller.h"
#include "movejobcontroller.h"
#include "trashjobcontroller.h"
#include "copyjobcontroller.h"
#include "renamejobcontroller.h"
#include "deletejobcontroller.h"
#include "dbuscontroller.h"
#include "fileconflictcontroller.h"
#include "views/global.h"


AppController::AppController(QObject *parent) : QObject(parent)
{
    m_menuController = new MenuController;
    m_trashJobController = new TrashJobController;
    m_moveJobController = new MoveJobController;
    m_copyJobController = new CopyJobController;
    m_renameJobController = new RenameJobController;
    m_deleteJobController = new DeleteJobController;
    m_fileConflictController = new FileConflictController;
}

void AppController::initConnect(){

}

AppController::~AppController()
{

}

