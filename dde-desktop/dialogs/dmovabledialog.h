#ifndef DMOVABLEDIALOG_H
#define DMOVABLEDIALOG_H


#include <QDialog>
#include <QPoint>
class QMouseEvent;


class DMovabelDialog:public QDialog
{
public:
    DMovabelDialog(QWidget *parent = 0);
    ~DMovabelDialog();

public slots:
    void setMovableHeight(int height);
    void moveCenter();

protected:
    void mouseMoveEvent(QMouseEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);

private:
    QPoint m_dragPosition;
    int m_movableHeight = 30;
};

#endif // DMOVABLEDIALOG_H
