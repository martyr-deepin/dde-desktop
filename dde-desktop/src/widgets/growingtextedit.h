#ifndef GROWINGTEXTEDIT_H
#define GROWINGTEXTEDIT_H

#include <QTextEdit>
#include <QKeyEvent>

class GrowingTextEdit : public QTextEdit
{
    Q_OBJECT
public:
    explicit GrowingTextEdit(QWidget *parent = 0);
    ~GrowingTextEdit();

signals:

public slots:
    void sizeChange();

protected:
    void keyPressEvent(QKeyEvent* event);

private:
    int m_minHeight = 0;
    int m_maxHeight = 6000;
};

#endif // GROWINGTEXTEDIT_H
