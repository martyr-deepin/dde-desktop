#include "copyjobcontroller.h"
#include "views/global.h"
#include "dbuscontroller.h"
#include "dbusinterface/copyjob_interface.h"
#include "dbusinterface/fileoperations_interface.h"
#include "dbusinterface/services/conflictdaptor.h"
#include "copyjobworker.h"


CopyJobController::CopyJobController(QObject *parent) : QObject(parent)
{
    initConnect();
}

void CopyJobController::initConnect(){
    connect(signalManager, SIGNAL(copyFilesExcuted(QStringList,QString)),
            this, SLOT(createCopyJob(QStringList,QString)));
}

void CopyJobController::createCopyJob(QStringList files, QString destination){
    CopyjobWorker* worker = new CopyjobWorker(files, destination);
    connect(worker, SIGNAL(finished()), worker, SLOT(deleteLater()));
    emit worker->startJob();
    qDebug() << worker;
}

CopyJobController::~CopyJobController()
{

}

