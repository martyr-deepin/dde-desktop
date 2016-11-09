/**
 * Copyright (C) 2016 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/
#include "screenframe.h"

#include <QApplication>
#include <QScreen>
#include <QDebug>

#include "util/xcb/xcb.h"

#include "canvasgridview.h"

#include <durl.h>
#include <diconitemdelegate.h>
#include "canvasviewhelper.h"

ScreenFrame::ScreenFrame(QWidget *parent) : QWidget(parent)
{
    setAttribute(Qt::WA_TranslucentBackground);
    Xcb::XcbMisc::instance().set_window_type(winId(), Xcb::XcbMisc::Desktop);

    setGeometry(qApp->primaryScreen()->geometry());

    wc = new CanvasGridView(this);;
    wc->setGeometry(qApp->primaryScreen()->availableGeometry());
    wc->setRootUrl(DUrl::fromLocalFile("/home/iceyer/Desktop"));

    connect(qApp->primaryScreen(), &QScreen::availableGeometryChanged, this, [ = ](const QRect & geometry) {
        qDebug() << "Screen geometry changed" << geometry;
        qDebug() << qApp->primaryScreen()->geometry()
                 << qApp->primaryScreen()->availableGeometry();
        setGeometry(qApp->primaryScreen()->availableGeometry());
        wc->resize(size());
    });
}

QSize ScreenFrame::canvasSize()
{
    return wc->size();
}

void ScreenFrame::bindPresenter(WidgetPresenter *)
{

}

void ScreenFrame::setModel(QAbstractItemModel *model)
{
//    wc->setModel(model);
}
