#include "appcontroller.h"
#include "menucontroller.h"
#include "movejobcontroller.h"
#include "trashjobcontroller.h"
#include "copyjobcontroller.h"
#include "dbuscontroller.h"
#include "views/global.h"


AppController::AppController(QObject *parent) : QObject(parent)
{
    m_dbusController = DBusController::instance();
    m_menuController = new MenuController;
    m_trashJobController = new TrashJobController;
    m_moveJobController = new MoveJobController;
    m_copyJobController = new CopyJobController;
}

void AppController::initConnect(){

}

AppController::~AppController()
{

}

