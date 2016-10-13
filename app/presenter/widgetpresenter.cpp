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

#include "../view/widgetcanvas.h"
#include "../../plugin/interface/presenterproxy.h"

WidgetPresenter::WidgetPresenter(QObject *parent) : QObject(parent)
{
    m_proxy = new PresenterProxy(this);
}

void WidgetPresenter::initModel(const QSize &canvasSize)
{
    auto cellSize = DDE::Desktop::CellSize128;

    auto rowCount = canvasSize.width() / cellSize.width();
    auto colCount = canvasSize.height() / cellSize.height();

    m_model = new QStandardItemModel(rowCount, colCount, this);
}

QStandardItemModel *WidgetPresenter::model()
{
    return m_model;
}

PresenterProxy &WidgetPresenter::proxy()
{
    return *m_proxy;
}

void WidgetPresenter::registerPresenterProxy(PresenterProxy *proxy)
{
    qDebug() << proxy;
    connect(proxy, &PresenterProxy::addWidget, this, &WidgetPresenter::addWidget);
    qDebug() << proxy;
}
#include <QTimer>
void WidgetPresenter::addWidget(QVariant data, QWidget *w)
{
//    m_model->free();
    qDebug() << data << w;
    m_model->setData(m_model->index(0, 1), QVariant::fromValue(w));

    QTimer::singleShot(2000, this, [ = ]() {
        m_model->setData(m_model->index(1, 1), QVariant::fromValue(w));
    });

}

void WidgetPresenter::updateModelWithSizeChanged(const QSize &cell, const QSize &canvas)
{

}
