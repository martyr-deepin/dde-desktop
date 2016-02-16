/**
 * Copyright (C) 2015 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

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
