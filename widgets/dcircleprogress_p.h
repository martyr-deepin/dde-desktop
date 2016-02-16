/**
 * Copyright (C) 2015 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#ifndef DCIRCLEPROGRESSPRIVATE_H
#define DCIRCLEPROGRESSPRIVATE_H

#include "dcircleprogress.h"
#include "dobject.h"
#include "dobject_p.h"

#include <QPainter>


class DCircleProgressPrivate : public DObjectPrivate
{
    Q_DECLARE_PUBLIC(DCircleProgress)

public:
    DCircleProgressPrivate(DCircleProgress *q);

private:
    void paint(QPainter *painter);

private:
    int m_lineWidth = 3;
    int m_currentValue = 40;
    int m_minmumValue = 0;
    int m_maximumValue = 100;

    QLabel m_topLabel;
    QLabel m_bottomLabel;

    QColor m_chunkColor = Qt::cyan;
    QColor m_backgroundColor = Qt::darkCyan;
};

#endif // DCIRCLEPROGRESSPRIVATE_H
