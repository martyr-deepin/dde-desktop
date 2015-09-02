#ifndef BACKGROUNDLABEL_H
#define BACKGROUNDLABEL_H

#include <QLabel>
class QResizeEvent;
class SystemBackground;
class BackgroundLabel : public QLabel
{
    Q_OBJECT

public:
    BackgroundLabel(bool isBlur=false, QWidget *parent = 0);
    ~BackgroundLabel();

protected:
    void resizeEvent(QResizeEvent* event);

protected:
    void setBlur(bool isBlur);

private:
    SystemBackground* m_systemBackground;
};

#endif // BACKGROUNDLABEL_H
