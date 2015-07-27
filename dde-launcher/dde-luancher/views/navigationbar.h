#ifndef NAVIGATIONBAR_H
#define NAVIGATIONBAR_H

#include <QFrame>

class NavigationBar : public QFrame
{
    Q_OBJECT
public:
    explicit NavigationBar(QWidget *parent = 0);
    ~NavigationBar();

signals:

public slots:
};

#endif // NAVIGATIONBAR_H
