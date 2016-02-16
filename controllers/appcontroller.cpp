/**
 * Copyright (C) 2015 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

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


AppController::AppController(QObject *parent) : QObject(parent),
    m_menuController(new MenuController),
    m_trashJobController(new TrashJobController),
    m_moveJobController(new MoveJobController),
    m_copyJobController(new CopyJobController),
    m_renameJobController(new RenameJobController),
    m_deleteJobController(new DeleteJobController)
{

}

void AppController::initConnect(){

}

MenuController* AppController::getMenuController(){
    return m_menuController;
}

TrashJobController* AppController::getTrashJobController(){
    return m_trashJobController;
}

MoveJobController* AppController::getMoveJobController(){
    return m_moveJobController;
}

CopyJobController* AppController::getCopyJobController(){
    return m_copyJobController;
}

RenameJobController* AppController::getRenameJobController(){
    return m_renameJobController;
}

AppController::~AppController()
{

}

