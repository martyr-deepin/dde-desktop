#ifndef APPGROUPBOX_H
#define APPGROUPBOX_H

#include <libdui/darrowrectangle.h>

#include "desktopfwd.h"

DUI_USE_NAMESPACE

class AppGroupBox : public DArrowRectangle
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
