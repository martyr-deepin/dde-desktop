/**
 * Copyright (C) 2015 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#include "deletejobcontroller.h"
#include "dbusinterface/deletejob_interface.h"
#include "dbusinterface/fileoperations_interface.h"
#include "dialogs/confirmdeletedialog.h"
#include "views/global.h"
#include "deletejobworker.h"

DeleteJobController::DeleteJobController(QObject *parent) : QObject(parent)
{
    initConnect();
}

void DeleteJobController::initConnect(){
    connect(signalManager, SIGNAL(filesDeleted(QStringList)),
            this, SLOT(createDeleteJob(QStringList)));
}

void DeleteJobController::createDeleteJob(const QStringList &files){
    DeletejobWorker* worker = new DeletejobWorker(files);
    connect(worker, SIGNAL(finished()), worker, SLOT(deleteLater()));
    emit worker->startJob();
    qDebug() << worker;
}

DeleteJobController::~DeleteJobController()
{

}

