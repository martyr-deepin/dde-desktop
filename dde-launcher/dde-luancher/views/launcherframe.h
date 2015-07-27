#ifndef LAUNCHERFRAME_H
#define LAUNCHERFRAME_H

#include <QFrame>
#include <QMouseEvent>

class LauncherFrame : public QFrame
{
    Q_OBJECT
public:
    explicit LauncherFrame(QWidget *parent = 0);
    ~LauncherFrame();

    void initUI();
    void initConnect();

signals:

public slots:

protected:
    void mouseReleaseEvent(QMouseEvent* event);
    void keyPressEvent(QKeyEvent* event);
};

#endif // LAUNCHERFRAME_H
