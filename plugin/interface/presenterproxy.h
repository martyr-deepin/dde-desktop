/**
 * Copyright (C) 2016 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#pragma once

#include <QObject>
#include <QVariant>

class PresenterProxy : public QObject
{
    Q_OBJECT
public:
    explicit PresenterProxy(QObject *parent = 0): QObject(parent) {}

signals:
    void addWidget(QVariant data, QWidget *w);
};

