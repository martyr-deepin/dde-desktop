#include "appcontroller.h"
#include "views/global.h"

AppController::AppController(QObject *parent) : QObject(parent)
{
    m_menuController = new MenuController;
}

void AppController::initConnect(){

}

AppController::~AppController()
{

}

