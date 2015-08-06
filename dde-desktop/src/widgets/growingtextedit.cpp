#include "growingtextedit.h"
#include <QDebug>

GrowingTextEdit::GrowingTextEdit(QWidget *parent) : QTextEdit(parent)
{
    connect(document(), SIGNAL(contentsChanged()), this, SLOT(sizeChange()));
}

void GrowingTextEdit::sizeChange(){
    int docHeight = document()->size().height();
    if (docHeight >= m_minHeight && docHeight <= m_maxHeight){
       setFixedHeight(docHeight);
    }
}

void GrowingTextEdit::keyPressEvent(QKeyEvent *event){
    if (event->modifiers() == Qt::NoModifier && event->key() == Qt::Key_Escape){
        hide();
        return;
    }
    QTextEdit::keyPressEvent(event);
}

GrowingTextEdit::~GrowingTextEdit()
{

}

