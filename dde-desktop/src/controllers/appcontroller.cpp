#include "appcontroller.h"
#include "views/global.h"

AppController::AppController(QObject *parent) : QObject(parent)
{
    m_menuController = new MenuController;
    m_dbusController = DBusController::instance();
}

void AppController::initConnect(){

}

AppController::~AppController()
{

}

