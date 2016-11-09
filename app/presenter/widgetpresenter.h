/**
 * Copyright (C) 2016 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#ifndef WIDGETPRESENTER_H
#define WIDGETPRESENTER_H

#include <QObject>

#include <QAbstractItemModel>

class QStandardItemModel;
class PresenterProxy;

class WidgetPresenter : public QObject
{
    Q_OBJECT
public:
    explicit WidgetPresenter(QObject *parent = 0);

    void initModel(const QSize &canvasSize);

    PresenterProxy &proxy();
    void registerPresenterProxy(PresenterProxy *proxy);

signals:

    // presenter proxy
public slots:
//    void removeWidget(QWidget *);
//    void updateWidget(QWidget *);

public slots:
    void updateModelWithSizeChanged(const QSize &cell, const QSize &canvas);

private:
    PresenterProxy      *m_proxy = nullptr;
};

#endif // WIDGETPRESENTER_H
