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

class DFileSystemModel;
class CanvasGridPresenter : public QObject
{
    Q_OBJECT
public:
    explicit CanvasGridPresenter(QObject *parent = 0);

    void init();

    DFileSystemModel *model() const;
private:
    void initModel();
};

#endif // WIDGETPRESENTER_H
