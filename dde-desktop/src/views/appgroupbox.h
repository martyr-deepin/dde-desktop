#ifndef APPGROUPBOX_H
#define APPGROUPBOX_H

#include <QtCore>
#include <QtGui>
#include <QtWidgets>
#include "desktopitem.h"
#include "global.h"
#include "widgets/arrowrectangle.h"

class AppGroupBox : public ArrowRectangle
{
    Q_OBJECT
public:
    explicit AppGroupBox(QWidget *parent = 0);
    ~AppGroupBox();

signals:

public slots:
    void showDetailByDesktopItem(DesktopItemPointer& pItem);

};

#endif // APPGROUPBOX_H
