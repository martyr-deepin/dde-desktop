#ifndef TRANSLUCENTFRAME_H
#define TRANSLUCENTFRAME_H

#include <QtCore>
#include <QtGui>
#include <QtWidgets>

class TranslucentFrame : public QFrame
{
    Q_OBJECT
public:
    explicit TranslucentFrame(QWidget *parent = 0);
    ~TranslucentFrame();

signals:

public slots:
};

#endif // TRANSLUCENTFRAME_H
