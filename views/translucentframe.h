/**
 * Copyright (C) 2015 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#ifndef TRANSLUCENTFRAME_H
#define TRANSLUCENTFRAME_H

#include <QtCore>
#include <QtGui>
#include <QtWidgets>

class TranslucentFrame : public QFrame
{
    Q_OBJECT
public:
    explicit TranslucentFrame(QWidget *parent = 0);
    ~TranslucentFrame();

signals:

public slots:

};

#endif // TRANSLUCENTFRAME_H
