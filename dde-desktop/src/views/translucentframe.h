#ifndef TRANSLUCENTFRAME_H
#define TRANSLUCENTFRAME_H

#include <QFrame>
#include <QWidget>

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
