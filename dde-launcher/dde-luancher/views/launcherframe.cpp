#include "launcherframe.h"
#include "util.h"
#include <QApplication>
#include <QDesktopWidget>

LauncherFrame::LauncherFrame(QWidget *parent) : QFrame(parent)
{
    setAttribute(Qt::WA_DeleteOnClose);
    setWindowFlags(Qt::FramelessWindowHint);
    setWindowState(Qt::WindowFullScreen);
    setFixedSize(qApp->desktop()->screenGeometry().size());
    setObjectName("LauncherFrame");

    initUI();
    initConnect();

    setStyleSheet(getQssFromFile(":/skin/qss/main.qss"));
}


void LauncherFrame::initUI(){

}


void LauncherFrame::initConnect(){

}

void LauncherFrame::mouseReleaseEvent(QMouseEvent *event){

}

void LauncherFrame::keyPressEvent(QKeyEvent *event){
    if (event->key() == Qt::Key_Escape){
        close();
    }
}

LauncherFrame::~LauncherFrame()
{

}

