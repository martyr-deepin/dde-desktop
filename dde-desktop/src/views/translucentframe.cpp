#include "views/translucentframe.h"

TranslucentFrame::TranslucentFrame(QWidget *parent) : QFrame(parent)
{
    setAttribute(Qt::WA_TranslucentBackground);
//    setWindowOpacity(0.5);
    setAttribute(Qt::WA_DeleteOnClose);
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnBottomHint);
    setWindowState(Qt::WindowMaximized);
    setGeometry(qApp->desktop()->availableGeometry());
}
        
TranslucentFrame::~TranslucentFrame()
{
//    qDebug() << "delete";
}
