/**
 * Copyright (C) 2015 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#include "dbusworker.h"
#include "views/global.h"
#include <QTimer>

DBusWorker::DBusWorker(QObject *parent) :
    QObject(parent)
{
    m_requestTimer = new QTimer(this);
    m_requestTimer->setInterval(500);
    connect(m_requestTimer, SIGNAL(timeout()),
            this, SLOT(loadDesktopItems()));
    connect(signalManager, SIGNAL(startRequest()),
            this, SLOT(start()));
    connect(signalManager, SIGNAL(stopRequest()),
            this, SLOT(stop()));
}

void DBusWorker::start(){
    loadDesktopItems();
//    m_requestTimer->start();
}

void DBusWorker::stop(){
    disconnect(m_requestTimer, SIGNAL(timeout()),
            this, SLOT(loadDesktopItems()));
    m_requestTimer->stop();

}

void DBusWorker::loadDesktopItems(){
    m_count += 1;

    if (m_count < 100){
        dbusController->loadDesktopItems();
    }else{
        stop();
    }
}
