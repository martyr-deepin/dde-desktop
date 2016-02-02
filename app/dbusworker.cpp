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
    m_requestTimer->start();
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
