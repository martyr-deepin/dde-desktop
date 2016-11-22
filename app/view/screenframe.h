/**
 * Copyright (C) 2016 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/
#ifndef QSCREENFRAME_H
#define QSCREENFRAME_H

#include <QFrame>

class QAbstractItemModel;
class CanvasGridView;
class CanvasGridPresenter;

class ScreenFrame : public QFrame
{
    Q_OBJECT
public:
    explicit ScreenFrame(QWidget *parent = 0);

    QSize canvasSize();

    void bindPresenter(CanvasGridPresenter *);
signals:

private:
    CanvasGridView *wc = nullptr;
};

#endif // QSCREENFRAME_H
