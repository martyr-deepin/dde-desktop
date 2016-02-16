/**
 * Copyright (C) 2015 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#ifndef APPCONTROLLER_H
#define APPCONTROLLER_H


#include <QObject>

class MenuController;
class TrashJobController;
class MoveJobController;
class CopyJobController;
class RenameJobController;
class DeleteJobController;


class AppController : public QObject
{
    Q_OBJECT
public:
    explicit AppController(QObject *parent = 0);
    ~AppController();

    void initConnect();
    MenuController* getMenuController();
    TrashJobController* getTrashJobController();
    MoveJobController* getMoveJobController();
    CopyJobController* getCopyJobController();
    RenameJobController* getRenameJobController();
    DeleteJobController* getDeleteJobController();
signals:

public slots:

private:
    MenuController* m_menuController;
    TrashJobController* m_trashJobController;
    MoveJobController* m_moveJobController;
    CopyJobController* m_copyJobController;
    RenameJobController* m_renameJobController;
    DeleteJobController* m_deleteJobController;
};

#endif // APPCONTROLLER_H
