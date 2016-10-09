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
#include <QLabel>
#include <QListView>
#include <QStringListModel>
#include <QDebug>

#include "util/xcb/xcb.h"

ScreenFrame::ScreenFrame(QWidget *parent) : QWidget(parent)
{
    setAttribute(Qt::WA_TranslucentBackground);
    setGeometry(qApp->primaryScreen()->availableGeometry());

    Xcb::XcbMisc::instance().set_window_type(winId(), Xcb::XcbMisc::Desktop);

    QListView *a = new QListView(this);
    a->setViewMode(QListView::IconMode);

    auto model = new QStringListModel(this);

    // Make data
    QStringList List;
    List << "Test" << "Module" << "Iceyer";

    // Populate our model
    model->setStringList(List);

    // Glue model and view together
    a->setModel(model);
    a->resize(size());
    a->setAutoFillBackground(false);
    a->setStyleSheet("background-color: transparent;font-size: 128px; color:red;");

    connect(qApp->primaryScreen(), &QScreen::availableGeometryChanged, this, [=](const QRect &geometry) {
        qDebug() << geometry;
        setGeometry(qApp->primaryScreen()->availableGeometry());
        a->resize(size());
    });
}
