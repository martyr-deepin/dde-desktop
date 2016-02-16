/**
 * Copyright (C) 2015 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#include "views/translucentframe.h"
#include "app/xcb_misc.h"

TranslucentFrame::TranslucentFrame(QWidget *parent) : QFrame(parent)
{
    setAttribute(Qt::WA_TranslucentBackground);
    setAttribute(Qt::WA_DeleteOnClose);
    setGeometry(qApp->desktop()->availableGeometry());
}

TranslucentFrame::~TranslucentFrame()
{
//    qDebug() << "delete";
}
