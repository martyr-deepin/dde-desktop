/**
 * Copyright (C) 2016 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#include "widgetpresenter.h"

#include <QStandardItemModel>
#include <QDebug>

#include "../view/canvasview.h"
#include "../../plugin/interface/presenterproxy.h"

WidgetPresenter::WidgetPresenter(QObject *parent) : QObject(parent)
{
    m_proxy = new PresenterProxy(this);
}

void WidgetPresenter::initModel(const QSize &canvasSize)
{

}

PresenterProxy &WidgetPresenter::proxy()
{
    return *m_proxy;
}

void WidgetPresenter::registerPresenterProxy(PresenterProxy *proxy)
{
    qDebug() << proxy;
//    connect(proxy, &PresenterProxy::addWidget, this, &WidgetPresenter::addWidget);
    qDebug() << proxy;
}

void WidgetPresenter::updateModelWithSizeChanged(const QSize &cell, const QSize &canvas)
{

}
