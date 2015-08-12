#include "dmovabledialog.h"
#include <QMouseEvent>
#include <QApplication>
#include <QDesktopWidget>

DMovabelDialog::DMovabelDialog(QWidget *parent):QDialog(parent)
{
    setWindowFlags(Qt::FramelessWindowHint);
}

void DMovabelDialog::setMovableHeight(int height){
    m_movableHeight = height;
}


void DMovabelDialog::moveCenter(){
    QRect qr = frameGeometry();
    QPoint cp = qApp->desktop()->availableGeometry().center();
    qr.moveCenter(cp);
    move(qr.topLeft());
}

void DMovabelDialog::mousePressEvent(QMouseEvent *event)
{
    if(event->button() & Qt::LeftButton)
    {
        m_dragPosition = event->globalPos() - frameGeometry().topLeft();
    }
    QDialog::mousePressEvent(event);
}

void DMovabelDialog::mouseReleaseEvent(QMouseEvent *event)
{
    QDialog::mouseReleaseEvent(event);
}

void DMovabelDialog::mouseMoveEvent(QMouseEvent *event)
{
    move(event->globalPos() - m_dragPosition);
    QDialog::mouseMoveEvent(event);
}


DMovabelDialog::~DMovabelDialog()
{

}

