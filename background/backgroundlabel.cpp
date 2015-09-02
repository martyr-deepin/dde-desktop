#include "backgroundlabel.h"
#include "systembackground.h"
#include <QApplication>
#include <QDesktopWidget>
#include <QDebug>

BackgroundLabel::BackgroundLabel(bool isBlur, QWidget* parent)
    : QLabel(parent)
{
    setWindowFlags(Qt::FramelessWindowHint);
    setGeometry(qApp->desktop()->screenGeometry());
    if (parent){
        m_systemBackground = new SystemBackground(parent->size(), isBlur, this);
    }else{
        QSize desktopSize = qApp->desktop()->screenGeometry().size();
        m_systemBackground = new SystemBackground(desktopSize, isBlur, this);
    }
    setPixmap(m_systemBackground->getBackground());
    connect(m_systemBackground, SIGNAL(backgroundChanged(QPixmap)),
            this, SLOT(setPixmap(QPixmap)));
}

void BackgroundLabel::setBlur(bool isBlur){
    m_systemBackground->setBlur(isBlur);
}

void BackgroundLabel::resizeEvent(QResizeEvent *event){
    m_systemBackground->setBackgroundSize(size());
    QLabel::resizeEvent(event);
}

BackgroundLabel::~BackgroundLabel()
{

}
