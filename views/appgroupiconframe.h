/**
 * Copyright (C) 2015 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#ifndef APPGROUPICONFRAME_H
#define APPGROUPICONFRAME_H

#include <QtCore>
#include <QtGui>
#include <QtWidgets>

class AppGroupIconFrame : public QFrame
{
    Q_OBJECT
public:
    explicit AppGroupIconFrame(QWidget *parent = 0);
    ~AppGroupIconFrame();
    static QPixmap getPixmap(QStringList icons);
    static QPixmap getPixmap(QList<QPixmap> icons);

protected:
    void paintEvent(QPaintEvent *e) Q_DECL_OVERRIDE;
};

#endif // APPGROUPICONFRAME_H
