#include "movejobcontroller.h"
#include "dbusinterface/movejob_interface.h"
#include "dbusinterface/fileoperations_interface.h"
#include "dbusinterface/services/conflictdaptor.h"
#include "views/global.h"
#include "movejobworker.h"

MoveJobController::MoveJobController(QObject *parent) : QObject(parent)
{
    initConnect();
}

void MoveJobController::initConnect(){
    connect(signalManager, SIGNAL(moveFilesExcuted(QStringList,QString)),
            this, SLOT(createMoveJob(QStringList,QString)));
}

void MoveJobController::createMoveJob(QStringList files, QString destination){
    MovejobWorker* worker = new MovejobWorker(files, destination);
    connect(worker, SIGNAL(finished()), worker, SLOT(deleteLater()));
    emit worker->startJob();
    qDebug() << worker;
}

MoveJobController::~MoveJobController()
{

}

