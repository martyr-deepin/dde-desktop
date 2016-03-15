/**
 * Copyright (C) 2015 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#ifndef APPGROUPBOX_H
#define APPGROUPBOX_H

#include <darrowrectangle.h>

#include "desktopfwd.h"

class AppGroupBox : public DTK_WIDGET_NAMESPACE::DArrowRectangle
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
