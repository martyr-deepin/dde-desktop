/**
 * Copyright (C) 2015 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#include "growingtextedit.h"
#include <QDebug>

GrowingTextEdit::GrowingTextEdit(QWidget *parent) : QTextEdit(parent)
{
    connect(document(), SIGNAL(contentsChanged()), this, SLOT(sizeChange()));
}

QString GrowingTextEdit::getUrl(){
    return m_url;
}

void GrowingTextEdit::setUrl(QString url){
    m_url = url;
}

void GrowingTextEdit::sizeChange(){
    int docHeight = document()->size().height();
    if (docHeight >= m_minHeight && docHeight <= m_maxHeight){
       setFixedHeight(docHeight);
    }
}

void GrowingTextEdit::keyPressEvent(QKeyEvent *event){
    qDebug() << event->modifiers() << event->key();
    if (event->modifiers() == Qt::NoModifier && event->key() == Qt::Key_Escape){
        emit escaped(m_url);
        hide();
        return;
    }else if ((event->modifiers() == Qt::NoModifier && event->key() == Qt::Key_Return)
              || (event->modifiers() == Qt::KeypadModifier && event->key() == Qt::Key_Enter)){
        emit editedFinished();
        hide();
        return;
    }
    QTextEdit::keyPressEvent(event);
}

GrowingTextEdit::~GrowingTextEdit()
{

}

